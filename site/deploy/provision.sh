#!/usr/bin/env bash
# One-time (and idempotent) host setup for cfromthegroundup.com, run as root
# under the shared lock from a checkout or prepared candidate:
#   /usr/local/sbin/with-deploy-lock site/deploy/provision.sh
#
# Creates, only where missing:
# - PostgreSQL 18 database `cfromthegroundup` with the host's usual roles:
#   cfromthegroundup_owner (no login; owns the database and schema `app`),
#   cfromthegroundup_migration (login; member of the owner, runs migrations),
#   cfromthegroundup_web (login; may only use schema `app` and read/write rows)
# - pg_hba.conf lines letting the two login roles connect over 127.0.0.1 only
# - root-only credentials in /etc/astro-apps/cfromthegroundup/{web,migration}.env
# - the nightly backup script, service and timer
# Existing passwords and files are left alone; nothing else on the host changes.
set -Eeuo pipefail
# shellcheck source=site/deploy/common.sh
source "$(dirname "$0")/common.sh"
require_lock
here=$(cd "$(dirname "$0")" && pwd)

DB=cfromthegroundup
OWNER=${DB}_owner
MIGRATION=${DB}_migration
WEB=${DB}_web
HBA=/etc/postgresql/18/main/pg_hba.conf

psql_admin() { runuser -u postgres -- psql -X -v ON_ERROR_STOP=1 -q "$@"; }
role_exists() { [[ $(psql_admin -At -c "select 1 from pg_roles where rolname = '$1'") == 1 ]]; }
new_password() { openssl rand -hex 24; }

# --- Credentials: keep existing ones, otherwise generate and set them. -------
install -d -o root -g root -m 0700 "$ENV_DIR"
read_password() { sed -nE "s#^DATABASE_URL=\"?postgres://$1:([^@]+)@.*#\1#p" "$2" 2>/dev/null; }
web_password=$(read_password "$WEB" "$WEB_ENV" || true)
migration_password=$(read_password "$MIGRATION" "$MIGRATION_ENV" || true)
[[ -n $web_password ]] || web_password=$(new_password)
[[ -n $migration_password ]] || migration_password=$(new_password)

# --- Roles and database. -----------------------------------------------------
role_exists "$OWNER" || psql_admin -c "create role $OWNER nologin"
role_exists "$MIGRATION" || psql_admin -c "create role $MIGRATION login in role $OWNER"
role_exists "$WEB" || psql_admin -c "create role $WEB login"
# Passwords go in through stdin so they never appear in a process list.
psql_admin <<SQL
alter role $MIGRATION with login password '$migration_password';
alter role $WEB with login password '$web_password';
alter role $MIGRATION set search_path = app;
alter role $WEB set search_path = app;
SQL
if [[ $(psql_admin -At -c "select 1 from pg_database where datname = '$DB'") != 1 ]]; then
  psql_admin -c "create database $DB owner $OWNER"
fi
psql_admin -d "$DB" <<SQL
revoke all on database $DB from public;
grant connect on database $DB to $MIGRATION, $WEB;
create schema if not exists app authorization $OWNER;
grant usage on schema app to $WEB;
alter default privileges for role $OWNER in schema app grant select, insert, update, delete on tables to $WEB;
alter default privileges for role $OWNER in schema app grant select, usage on sequences to $WEB;
SQL

# --- pg_hba.conf: local TCP only, inserted before the catch-all rejects. -----
if ! grep -qE "^host[[:space:]]+$DB[[:space:]]+$WEB[[:space:]]" "$HBA"; then
  cp -a "$HBA" "/root/pg_hba.conf.before-$DB-$(date -u +%Y%m%dT%H%M%SZ)"
  grep -qE '^host[[:space:]]+all[[:space:]]+all[[:space:]]+0\.0\.0\.0/0[[:space:]]+reject' "$HBA" || die "no catch-all reject line in $HBA to insert before"
  awk -v web="$WEB" -v migration="$MIGRATION" -v db="$DB" '
    !done && $1 == "host" && $2 == "all" && $3 == "all" && $4 == "0.0.0.0/0" && $5 == "reject" {
      print "host " db " " web " 127.0.0.1/32 scram-sha-256"
      print "host " db " " migration " 127.0.0.1/32 scram-sha-256"
      done = 1
    }
    { print }
  ' "$HBA" > "$HBA.next"
  chown --reference="$HBA" "$HBA.next"
  chmod --reference="$HBA" "$HBA.next"
  mv -f "$HBA.next" "$HBA"
  pg_ctlcluster 18 main reload
fi

# --- Environment files (root-only; systemd reads them as root). --------------
umask 077
printf 'DATABASE_URL=postgres://%s:%s@127.0.0.1:5432/%s\n' "$WEB" "$web_password" "$DB" > "$WEB_ENV.next"
printf 'DATABASE_URL=postgres://%s:%s@127.0.0.1:5432/%s\nDB_OWNER_ROLE=%s\n' "$MIGRATION" "$migration_password" "$DB" "$OWNER" > "$MIGRATION_ENV.next"
chmod 0600 "$WEB_ENV.next" "$MIGRATION_ENV.next"
mv -f "$WEB_ENV.next" "$WEB_ENV"
mv -f "$MIGRATION_ENV.next" "$MIGRATION_ENV"

# Prove both roles can log in over TCP with the stored credentials.
for env_file in "$WEB_ENV" "$MIGRATION_ENV"; do
  url=$(sed -nE 's/^DATABASE_URL=//p' "$env_file")
  PGCONNECT_TIMEOUT=5 runuser -u nobody -- env "DATABASE_URL=$url" sh -c 'psql -X -At "$DATABASE_URL" -c "select current_user"' \
    || die "cannot connect with $env_file"
done

# --- Nightly backup. ----------------------------------------------------------
install -m 0755 -o root -g root "$here/cfromthegroundup-backup" /usr/local/sbin/cfromthegroundup-backup
install -m 0644 -o root -g root "$here/cfromthegroundup-backup.service" "$here/cfromthegroundup-backup.timer" /etc/systemd/system/
systemctl daemon-reload
systemctl enable --now --quiet cfromthegroundup-backup.timer

echo "Provisioned database $DB, credentials in $ENV_DIR, and the nightly backup timer."
