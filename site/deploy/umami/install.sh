#!/usr/bin/env bash
# Install or update self-hosted Umami analytics for cfromthegroundup.com
# (BUILD.md W9, decision D9). Run as root under the shared lock, from a
# checkout of this repository on the server:
#
#   /usr/local/sbin/with-deploy-lock site/deploy/umami/install.sh
#
# Idempotent. Creates, only where missing:
# - the system user `umami`
# - PostgreSQL 18 role and database `umami` (the role owns the database, so
#   Umami's own build can create its tables), reachable over 127.0.0.1 only
# - root-only /etc/web-apps/umami/umami.env (DATABASE_URL and APP_SECRET)
# Then builds the pinned Umami release (UMAMI_VERSION) as the `umami` user in
# /srv/www/umami/releases/<version>, points /srv/www/umami/current at it,
# installs umami.service and waits until the tracker script answers.
#
# Nothing else on the host changes: the site's Caddy file (deployed with the
# site) already passes /stats/script.js and /stats/api/send to port 4321.
# The first build creates the Umami login admin / umami: change it at once
# (README.md, "Analytics").
set -Eeuo pipefail
here=$(cd "$(dirname "$0")" && pwd)
# shellcheck source=site/deploy/common.sh
source "$here/../common.sh"
require_lock

UMAMI_VERSION=v3.4.1
UMAMI_ROOT=/srv/www/umami
UMAMI_ENV_DIR=/etc/web-apps/umami
UMAMI_ENV=$UMAMI_ENV_DIR/umami.env
UMAMI_UNIT=/etc/systemd/system/umami.service
HBA=/etc/postgresql/18/main/pg_hba.conf
release=$UMAMI_ROOT/releases/$UMAMI_VERSION

psql_admin() { runuser -u postgres -- psql -X -v ON_ERROR_STOP=1 -q "$@"; }

# --- The port must be free, or already ours. ---------------------------------
if ss -Hltn "sport = :$UMAMI_PORT" | grep -q .; then
  systemctl is-active --quiet umami.service || die "port $UMAMI_PORT is taken by something other than umami.service"
fi

# --- System user. -------------------------------------------------------------
id umami >/dev/null 2>&1 || useradd --system --home-dir /nonexistent --shell /usr/sbin/nologin umami

# --- Credentials: keep existing ones. -----------------------------------------
install -d -o root -g root -m 0700 "$UMAMI_ENV_DIR"
password=$(sed -nE 's#^DATABASE_URL="?postgresql://umami:([^@]+)@.*#\1#p' "$UMAMI_ENV" 2>/dev/null || true)
secret=$(sed -nE 's#^APP_SECRET=##p' "$UMAMI_ENV" 2>/dev/null || true)
[[ -n $password ]] || password=$(openssl rand -hex 24)
[[ -n $secret ]] || secret=$(openssl rand -hex 32)
database_url="postgresql://umami:$password@127.0.0.1:5432/umami"

# --- Role and database. ---------------------------------------------------------
[[ $(psql_admin -At -c "select 1 from pg_roles where rolname = 'umami'") == 1 ]] || psql_admin -c 'create role umami login'
# The password goes in through stdin so it never appears in a process list.
psql_admin <<SQL
alter role umami with login password '$password';
SQL
if [[ $(psql_admin -At -c "select 1 from pg_database where datname = 'umami'") != 1 ]]; then
  psql_admin -c 'create database umami owner umami'
fi
psql_admin -d umami -c 'revoke all on database umami from public'

# --- pg_hba.conf: local TCP only, before the catch-all rejects. ----------------
if ! grep -qE '^host[[:space:]]+umami[[:space:]]+umami[[:space:]]' "$HBA"; then
  cp -a "$HBA" "/root/pg_hba.conf.before-umami-$(date -u +%Y%m%dT%H%M%SZ)"
  grep -qE '^host[[:space:]]+all[[:space:]]+all[[:space:]]+0\.0\.0\.0/0[[:space:]]+reject' "$HBA" || die "no catch-all reject line in $HBA to insert before"
  awk '
    !done && $1 == "host" && $2 == "all" && $3 == "all" && $4 == "0.0.0.0/0" && $5 == "reject" {
      print "host umami umami 127.0.0.1/32 scram-sha-256"
      done = 1
    }
    { print }
  ' "$HBA" > "$HBA.next"
  chown --reference="$HBA" "$HBA.next"
  chmod --reference="$HBA" "$HBA.next"
  mv -f "$HBA.next" "$HBA"
fi
[[ $(psql_admin -At -c "select count(*) from pg_hba_file_rules where error is not null") == 0 ]] || die "$HBA has errors (see pg_hba_file_rules); not reloading"
psql_admin -At -c 'select pg_reload_conf()' >/dev/null

# --- Environment file (root-only; systemd reads it as root). -------------------
umask 077
printf 'DATABASE_URL=%s\nAPP_SECRET=%s\n' "$database_url" "$secret" > "$UMAMI_ENV.next"
chmod 0600 "$UMAMI_ENV.next"
mv -f "$UMAMI_ENV.next" "$UMAMI_ENV"
umask 022

# --- Build the pinned release as the umami user. -------------------------------
install -d -o root -g root -m 0755 "$UMAMI_ROOT" "$UMAMI_ROOT/releases"
if [[ ! -f $release/.built ]]; then
  work=$UMAMI_ROOT/releases/.build-$UMAMI_VERSION
  rm -rf "$work"
  install -d -o umami -g umami -m 0750 "$work"
  as_umami() {
    runuser -u umami -- env -i HOME="$work/.home" PATH=/opt/node-runtime/bin:/usr/bin:/bin \
      COREPACK_ENABLE_DOWNLOAD_PROMPT=0 DISABLE_TELEMETRY=1 \
      DATABASE_URL="$database_url" APP_SECRET="$secret" "$@"
  }
  as_umami git clone --quiet --depth 1 --branch "$UMAMI_VERSION" https://github.com/umami-software/umami.git "$work/app"
  # The first build creates Umami's tables (and the admin / umami login).
  ( cd "$work/app" && as_umami corepack pnpm install --frozen-lockfile && as_umami corepack pnpm build ) \
    || die "building Umami $UMAMI_VERSION failed; $work is kept for inspection"
  rm -rf "$work/app/.next/cache"
  ln -s /var/cache/umami "$work/app/.next/cache"
  touch "$work/app/.built"
  # Read-only for the service: root owns the code, the umami group may read it.
  chown -R root:umami "$work/app"
  chmod -R g+rX,o-rwx "$work/app"
  rm -rf "$release" "$work/.home"
  mv "$work/app" "$release"
  rmdir "$work"
fi
ln -sfn "releases/$UMAMI_VERSION" "$UMAMI_ROOT/current.next"
mv -Tf "$UMAMI_ROOT/current.next" "$UMAMI_ROOT/current"

# --- Service. ----------------------------------------------------------------------
install -m 0644 -o root -g root "$here/umami.service" "$UMAMI_UNIT"
systemctl daemon-reload
systemctl enable --quiet umami.service
systemctl restart umami.service
for _ in {1..60}; do
  if curl -fsS -o /dev/null --max-time 5 "http://127.0.0.1:$UMAMI_PORT/script.js"; then
    echo "Umami $UMAMI_VERSION is running on 127.0.0.1:$UMAMI_PORT."
    echo "Next: change the admin password and add the website (README.md, \"Analytics\")."
    exit 0
  fi
  sleep 2
done
die "Umami did not answer on 127.0.0.1:$UMAMI_PORT (journalctl -u umami)"
