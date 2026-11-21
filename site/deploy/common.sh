#!/usr/bin/env bash
# Shared shell glue for cfromthegroundup.com deploys on ubuntu-vm. Sourced by
# prepare.sh, cutover.sh and provision.sh; see README.md. The flow is the
# host's web platform contract (ubuntu-vm-backup docs/web-platform.md):
# build and test outside the lock, then stage, migrate, check a candidate,
# switch and verify inside it. Fix forward only.
# shellcheck shell=bash disable=SC2034
set -Eeuo pipefail

NAME=cfromthegroundup
HOST=cfromthegroundup.com
SRV=/srv/www/cfromthegroundup.com
# The SvelteKit server (adapter-node) on loopback. Other sites' ports are in
# the contract's port table; check with `ss -ltn` and /etc/caddy/sites before
# taking a new one. Release candidates use a free port in 4400-4499.
PORT=4317
CANDIDATE_PORTS=(4400 4499)
# Umami analytics (site/deploy/umami/), which the Caddy file proxies /stats/ to.
UMAMI_PORT=4321
FRAGMENT=cfromthegroundup.com
SITE=/etc/caddy/sites/$FRAGMENT.caddy
SERVICE=cfromthegroundup.service
UNIT=/etc/systemd/system/$SERVICE
CANDIDATE=cfromthegroundup-candidate.service
# Root-only database credentials: web.env (DATABASE_URL for the app) and
# migration.env (DATABASE_URL and DB_OWNER_ROLE for migrations).
ENV_DIR=/etc/web-apps/cfromthegroundup
WEB_ENV=$ENV_DIR/web.env
MIGRATION_ENV=$ENV_DIR/migration.env
# Node 26 from the platform path, for runtime and for host-side checks. Builds
# use the /usr/local/bin/node-26 wrapper, which also puts pnpm on PATH.
NODE=/opt/node-26/bin/node
NODE_VERSION=26.13.1
# Pre-migration dumps (root-only).
DUMP_DIR=/var/backups/$HOST
# The prepared release inside a candidate directory.
RELEASE_DIR=release

die() { echo "ERROR: $*" >&2; exit 1; }
step() { printf '\n== %s\n' "$*"; }

# Privileged steps run only inside the host-wide lock, as descriptor 9.
require_lock() {
  [[ $EUID == 0 ]] || die 'run as root through /usr/local/sbin/with-deploy-lock'
  [[ $(readlink /proc/$$/fd/9) == /run/web-deploy/lock ]] || die 'shared deployment lock descriptor 9 is required'
  flock -n 9 || die 'shared deployment lock is not held'
}

# Things this deploy must never change: other sites and the shared Caddy setup.
preserved() {
  sha256sum /etc/caddy/Caddyfile /usr/local/lib/caddy/caddy
  find /etc/caddy/sites -maxdepth 1 -type f ! -name "$FRAGMENT.caddy" -print0 | sort -z | xargs -0 -r sha256sum
  find /etc/systemd/system -maxdepth 1 -type f -name '*.service' ! -name "$SERVICE" -print0 | sort -z | xargs -0 -r sha256sum
  find /srv/www -mindepth 2 -maxdepth 2 -name current ! -path "$SRV/current" -printf '%p -> %l\n' | sort
}

# Fetch URL in full (never `curl | grep -q`: under pipefail grep exits early
# and curl fails writing the rest). Retries while the server starts.
get() {
  local url=$1 attempt
  shift
  for attempt in {1..45}; do
    if curl -fsS --max-time 10 "$@" "$url"; then return 0; fi
    sleep 1
  done
  return 1
}

status_of() { curl -s -o /dev/null -w '%{http_code}' --max-time 10 "$@"; }

# /healthz must report this exact release and a database the web role reached.
#   check_health BASE_URL COMMIT [CURL_OPTION...]
check_health() {
  local base=$1 commit=$2 body
  shift 2
  body=$(get "$base/healthz" "$@") || die "$base/healthz did not answer 200"
  [[ $body == "{\"status\":\"ok\",\"revision\":\"$commit\",\"database\":\"ok\"}" ]] \
    || die "$base/healthz answered $body, expected revision $commit and database ok"
}

# The site's own smoke check of the Node server, the same for the candidate,
# the live port and (with --resolve options) the public URL through Caddy:
# main routes, accounts state, a 404.
#   smoke_app BASE_URL [CURL_OPTION...]
smoke_app() {
  local base=$1 home account path
  shift
  home=$(get "$base/" "$@") || die "$base/ failed"
  [[ $home == *'C From The Ground Up'* ]] || die "$base/ lacks the site title"
  for path in /lessons /lessons/01-hello-world /setup /privacy /about /glossary; do
    [[ $(status_of "$@" "$base$path") == 200 ]] || die "$base$path did not answer 200"
  done
  [[ $(status_of "$@" "$base/no-such-page") == 404 ]] || die "$base/no-such-page is not a 404"
  account=$(get "$base/api/account" "$@") || die "$base/api/account failed"
  [[ $account == *'"enabled":'* ]] || die "$base/api/account returned $account"
}

# What the public sees, through the local Caddy with the real hostname and
# certificate: the same app checks plus what only Caddy does (headers, the
# lesson files' Content-Type, the client address, the www redirect).
smoke_public() {
  local commit=$1 resolve=(--resolve "$HOST:443:127.0.0.1" --resolve "www.$HOST:443:127.0.0.1")
  local base=https://$HOST live seen lesson_type headers
  check_health "$base" "$commit" "${resolve[@]}"
  smoke_app "$base" "${resolve[@]}"
  live=$(get "$base/version.txt" "${resolve[@]}") || die 'version.txt failed through Caddy'
  [[ $live == "$commit" ]] || die "live version.txt is $live, expected $commit"
  headers=$(curl -fsS -D - -o /dev/null --max-time 10 "${resolve[@]}" "$base/") || die 'headers failed through Caddy'
  [[ ${headers,,} == *'strict-transport-security:'* && ${headers,,} == *'x-content-type-options: nosniff'* ]] \
    || die 'security headers missing through Caddy'
  # The app must see the address Caddy saw (here loopback), never one the client
  # claims in X-Forwarded-For: rate limiting depends on it.
  seen=$(curl -fsS --max-time 10 "${resolve[@]}" -H 'X-Forwarded-For: 203.0.113.9' "$base/healthz/address") || die '/healthz/address failed through Caddy'
  [[ $seen == '{"address":"127.0.0.1"}' ]] || die "client address through Caddy is $seen, expected 127.0.0.1"
  [[ $(status_of "${resolve[@]}" "https://www.$HOST/") == 301 ]] || die 'www does not redirect to the apex'
  # Lesson files open as text in the browser (Caddy serves them from disk).
  lesson_type=$(curl -fsS -o /dev/null -w '%{content_type}' --max-time 10 "${resolve[@]}" "$base/lessons/01-hello-world/files/1_hello_world.c") || die 'lesson file download failed through Caddy'
  [[ $lesson_type == 'text/plain; charset=utf-8' ]] || die "lesson file through Caddy has Content-Type $lesson_type"
  # /stats/ goes to Umami's port: it must be Umami or nothing, never another
  # site's app (4318 was once taken by mistake).
  if ss -Hltn "sport = :$UMAMI_PORT" | grep -q .; then
    systemctl is-active --quiet umami.service || die "port $UMAMI_PORT is in use by something other than umami.service"
  fi
}
