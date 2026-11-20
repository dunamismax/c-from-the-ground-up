#!/usr/bin/env bash
# Shared shell glue for cfromthegroundup.com deploys on ubuntu-vm. Sourced by
# prepare.sh, cutover.sh, rollback.sh and provision.sh; see README.md.
# shellcheck shell=bash disable=SC2034
set -Eeuo pipefail

NAME=cfromthegroundup
HOST=cfromthegroundup.com
SRV=/srv/www/cfromthegroundup.com
# The SvelteKit server (adapter-node) on loopback. 4311-4316 belong to other
# sites on this host.
PORT=4317
SITE=/etc/caddy/sites/cfromthegroundup.com.caddy
UNIT=/etc/systemd/system/cfromthegroundup.service
SERVICE=cfromthegroundup.service
# Root-only database credentials: web.env (DATABASE_URL for the app) and
# migration.env (DATABASE_URL and DB_OWNER_ROLE for migrations).
ENV_DIR=/etc/astro-apps/cfromthegroundup
WEB_ENV=$ENV_DIR/web.env
MIGRATION_ENV=$ENV_DIR/migration.env
# The host's shared runtime (Node 24) and custom Caddy build.
NODE=/opt/astro-runtime/bin/node
CADDY=/usr/local/lib/caddy/caddy
BACKUP_PREFIX=/root/cfromthegroundup-backup-

die() { echo "ERROR: $*" >&2; exit 1; }

# Privileged steps run only inside the host-wide lock, as descriptor 9.
require_lock() {
  [[ $EUID == 0 ]] || die 'run as root through /usr/local/sbin/with-deploy-lock'
  [[ $(readlink /proc/$$/fd/9) == /run/astro-deploy/lock ]] || die 'shared deployment lock descriptor 9 is required'
  flock -n 9 || die 'shared deployment lock is not held'
}

# Everything this site owns on the host. Cutover refuses to run if it changed
# between prepare and cutover.
snapshot() {
  if [[ -L $SRV/current ]]; then echo "current -> $(readlink -f "$SRV/current")"; else echo 'current absent'; fi
  if [[ -f $SITE ]]; then sha256sum "$SITE"; else echo "absent $SITE"; fi
  if [[ -f $UNIT ]]; then sha256sum "$UNIT"; else echo "absent $UNIT"; fi
}

# Things this deploy must never change: other sites and the shared Caddy setup.
preserved() {
  sha256sum /etc/caddy/Caddyfile "$CADDY"
  find /etc/caddy/sites -maxdepth 1 -type f ! -name "$(basename "$SITE")" -print0 | sort -z | xargs -0 -r sha256sum
  find /etc/systemd/system -maxdepth 1 -type f -name '*.service' ! -name "$SERVICE" -print0 | sort -z | xargs -0 -r sha256sum
  find /srv/www -mindepth 2 -maxdepth 2 -name current ! -path "$SRV/current" -printf '%p -> %l\n' | sort
}

validate_caddy() { runuser -u caddy -- env HOME=/var/lib/caddy "$CADDY" validate --config /etc/caddy/Caddyfile --adapter caddyfile; }

# Fetch a path from the local Caddy over HTTPS with the real hostname.
# Retries while Caddy obtains the certificate after the first reload.
local_get() {
  local path=$1 attempt
  for attempt in {1..45}; do
    if curl -fsS --max-time 10 --resolve "$HOST:443:127.0.0.1" "https://$HOST$path"; then return 0; fi
    sleep 2
  done
  return 1
}

# Install (or update) the unit file and restart it on the live release.
start_unit() {
  local file=$1
  install -m 0644 -o root -g root "$file" "$UNIT"
  systemctl daemon-reload
  systemctl enable --quiet "$SERVICE"
  systemctl restart "$SERVICE"
}

# Stop and remove the unit (absent is fine).
remove_unit() {
  systemctl disable --now --quiet "$SERVICE" 2>/dev/null || true
  rm -f "$UNIT"
  systemctl daemon-reload
}

# The server answers /healthz with 200 once it is up and the database answers.
wait_site_service() {
  local attempt
  for attempt in {1..45}; do
    if curl -fsS --max-time 3 "http://127.0.0.1:$PORT/healthz" >/dev/null; then return 0; fi
    sleep 1
  done
  return 1
}
