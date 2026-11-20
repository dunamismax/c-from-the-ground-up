#!/usr/bin/env bash
# Atomic, reversible cutover of a prepared release. Run as root under the
# shared lock:
#   /usr/local/sbin/with-deploy-lock CANDIDATE/site/deploy/cutover.sh CANDIDATE
# Writes only /srv/www/cfromthegroundup.com, the site's database (migrations), /etc/caddy/sites/cfromthegroundup.com.caddy,
# /etc/systemd/system/cfromthegroundup.service and a root-only backup in
# /root/cfromthegroundup-backup-*; restarts the site server and reloads Caddy.
# On any failure it restores the previous state through rollback.sh.
set -Eeuo pipefail
# shellcheck source=site/deploy/common.sh
source "$(dirname "$0")/common.sh"
require_lock
candidate=${1:?usage: cutover.sh CANDIDATE_DIRECTORY}
[[ -f $candidate/bundle/COMMIT && -f $candidate/bundle.sha256 && -f $candidate/expected-active ]] || die 'run site/deploy/prepare.sh first'
commit=$(cat "$candidate/bundle/COMMIT")
[[ $commit =~ ^[a-f0-9]{40}$ ]] || die 'invalid commit id'
cmp -s <(snapshot) "$candidate/expected-active" || die 'active release or site config changed since preparation; prepare again'
[[ -f $WEB_ENV ]] || die "missing $WEB_ENV; run site/deploy/provision.sh first"

backup="$BACKUP_PREFIX$(date -u +%Y%m%dT%H%M%SZ)-${commit:0:8}"
install -d -m 0700 "$backup" "$backup/tools"
cp -a "$candidate/bundle" "$backup/staged"
[[ -z $(find "$backup/staged" -type l -print) ]] || die 'candidate bundle must contain no symlinks'
(cd "$backup/staged" && sha256sum --quiet -c "$candidate/bundle.sha256")
chown -R root:root "$backup/staged"
cp -a "$(dirname "$0")"/*.sh "$backup/tools/"
# Record the state to restore: the previous release pointer, site file and
# unit, or their absence.
if [[ -L $SRV/current ]]; then readlink "$SRV/current" > "$backup/previous-current"; fi
if [[ -f $SITE ]]; then cp -a "$SITE" "$backup/site.caddy"; fi
if [[ -f $UNIT ]]; then cp -a "$UNIT" "$backup/site.service"; fi
preserved > "$backup/preserved-before"
touch "$backup/ready"
trap 'status=$?; if ((status != 0)); then echo "Cutover failed; restoring prior state" >&2; "$backup/tools/rollback.sh" "$backup"; fi' EXIT

# Stage the immutable release next to (not over) the live one.
install -d -m 0755 "$SRV" "$SRV/releases"
release="$SRV/releases/$commit"
if [[ -d $release ]]; then
  diff -qr "$backup/staged/web" "$release" >/dev/null || die "$release exists with different content"
else
  cp -a "$backup/staged/web" "$release.tmp"
  chown -R root:root "$release.tmp"
  find "$release.tmp" -type d -exec chmod 0755 {} +
  find "$release.tmp" -type f -exec chmod 0644 {} +
  mv -T "$release.tmp" "$release"
fi
[[ -f $release/build/index.js ]] || die 'release has no server build'
[[ $(cat "$release/build/client/version.txt") == "$commit" ]] || die 'release version.txt does not match the commit'
# Migrations are forward-only and run before the switch, so the new release
# never starts on an old schema. The live release keeps serving meanwhile, and
# a rollback restores code but not data, so every migration must also work
# for the release before it (add things; drop or rename only a release later).
# A full backup comes first.
if [[ -d $release/drizzle ]]; then
  [[ -f $release/build/migrate/migrate.js ]] || die 'release has migrations but no bundled migrator'
  /usr/local/sbin/cfromthegroundup-backup || die 'pre-migration backup failed'
  migrate_release "$release" || die 'migration failed; the backup above holds the database as it was'
fi

# Install and validate the site config before anything goes live.
install -m 0644 -o root -g root "$backup/staged/config/cfromthegroundup.com.caddy" "$SITE"
validate_caddy

# Switch atomically, restart the server on the new release, then reload Caddy
# (it obtains the certificates on first use).
ln -sfn "$release" "$SRV/current.next"
mv -Tf "$SRV/current.next" "$SRV/current"
start_unit "$backup/staged/config/cfromthegroundup.service"
wait_site_service || die "site server did not become healthy (journalctl -u $SERVICE)"
systemctl reload caddy

# Verify through Caddy with the real hostname and certificate.
live=$(local_get /version.txt) || die 'site did not answer over HTTPS'
[[ $live == "$commit" ]] || die "live version.txt is $live, expected $commit"
# Fetch each page in full before matching it. Piping curl into `grep -q`
# breaks under pipefail: grep exits at the first match, curl fails writing
# the rest (error 23), and local_get retries that until it gives up.
home=$(local_get /) || die 'home page failed through Caddy'
[[ $home == *'C From The Ground Up'* ]] || die 'home page through Caddy lacks the site title'
[[ $(local_get /healthz) == '{"status":"ok","database":"ok"}' ]] || die '/healthz failed through Caddy'
account=$(local_get /api/account) || die '/api/account failed through Caddy'
[[ $account == *'"enabled":'* ]] || die "/api/account through Caddy returned $account"
# The app must see the address Caddy saw (here loopback), never one the client
# claims in X-Forwarded-For: rate limiting depends on it.
seen=$(curl -fsS --max-time 10 --resolve "$HOST:443:127.0.0.1" -H 'X-Forwarded-For: 203.0.113.9' "https://$HOST/healthz/address") || die '/healthz/address failed through Caddy'
[[ $seen == '{"address":"127.0.0.1"}' ]] || die "client address through Caddy is $seen, expected 127.0.0.1"
[[ $(curl -s -o /dev/null -w '%{http_code}' --max-time 10 --resolve "www.$HOST:443:127.0.0.1" "https://www.$HOST/") == 301 ]] || die 'www does not redirect to the apex'

preserved > "$backup/preserved-after"
cmp -s "$backup/preserved-before" "$backup/preserved-after" || die 'another site or shared Caddy file changed during cutover'
[[ -z $(find "$release" \( -type f -o -type d \) \( ! -user root -o -perm /022 \) -print) ]] || die 'release ownership or permissions are wrong'

trap - EXIT
printf 'Deployed %s\n' "$commit"
printf 'Rollback: /usr/local/sbin/with-deploy-lock %s/tools/rollback.sh %s\n' "$backup" "$backup"
