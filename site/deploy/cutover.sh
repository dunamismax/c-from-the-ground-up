#!/usr/bin/env bash
# Put a prepared release live (steps 2-8 of the deploy flow, see README.md).
# Run as root under the shared lock, in one process:
#   /usr/local/sbin/with-deploy-lock CANDIDATE/src/site/deploy/cutover.sh CANDIDATE
#
# Writes only /srv/www/cfromthegroundup.com, the site's database (migrations),
# a pre-migration dump in /var/backups/cfromthegroundup.com,
# /etc/systemd/system/cfromthegroundup.service, a transient candidate unit in
# /run/systemd/system, and (only if it changed) the site's Caddy fragment
# through caddy-apply. Fix forward only: a failure stops at that step and says
# which one. Before the switch (steps 2-4) the previous release keeps serving;
# after it, fix the cause in a new commit and deploy that.
set -Eeuo pipefail
# shellcheck source=site/deploy/common.sh
source "$(dirname "$0")/common.sh"
require_lock
candidate=${1:?usage: cutover.sh CANDIDATE_DIRECTORY}
[[ -f $candidate/COMMIT && -f $candidate/release.sha256 && -d $candidate/$RELEASE_DIR ]] || die 'run site/deploy/prepare.sh first'
commit=$(cat "$candidate/COMMIT")
[[ $commit =~ ^[a-f0-9]{40}$ ]] || die 'invalid commit id'
[[ $(cat "$candidate/$RELEASE_DIR/REVISION") == "$commit" ]] || die 'prepared REVISION does not match COMMIT'
[[ -f $WEB_ENV && -f $MIGRATION_ENV ]] || die "missing $WEB_ENV or $MIGRATION_ENV"
current_step=start
trap 'status=$?; ((status == 0)) || echo "cutover stopped at step: $current_step (live release: $(readlink "$SRV/current" 2>/dev/null || echo none)); fix forward" >&2' EXIT
others_before=$(preserved)

# --- 2. Stage the immutable release. -------------------------------------------
current_step='2 stage'
step "$current_step"
release=$SRV/releases/$commit
install -d -m 0755 -o root -g root "$SRV" "$SRV/releases"
if [[ -d $release ]]; then
  (cd "$release" && sha256sum --quiet -c "$candidate/release.sha256") || die "$release exists with different content"
  [[ $(cd "$release" && find . -type f | wc -l) == $(wc -l < "$candidate/release.sha256") ]] || die "$release exists with extra files"
else
  rm -rf "$release.staging"
  cp -a "$candidate/$RELEASE_DIR" "$release.staging"
  chown -R root:root "$release.staging"
  chmod -R u=rwX,go=rX "$release.staging"
  (cd "$release.staging" && sha256sum --quiet -c "$candidate/release.sha256") || die 'staged release does not match the prepared checksums'
  mv -T "$release.staging" "$release"
fi
[[ -f $release/build/index.js && -f $release/build/migrate/migrate.js ]] || die 'release has no server build or migrator'
[[ $(cat "$release/.node-version") == "$NODE_VERSION" ]] || die "release .node-version is not $NODE_VERSION"
[[ $(cat "$release/build/client/version.txt") == "$commit" ]] || die 'release version.txt does not match the commit'
[[ -z $(find "$release" \( -type f -o -type d \) \( ! -user root -o -perm /022 \) -print) ]] || die 'release ownership or permissions are wrong'

# --- 3. Migrate, after a dump. ------------------------------------------------------
# Migrations are forward-only and run while the previous release still serves,
# so every migration must also work for that release (add things; drop or
# rename only a release later). The dump is for the owner's recovery decision,
# never restored by a deploy.
current_step='3 migrate'
step "$current_step"
install -d -m 0700 -o root -g root "$DUMP_DIR"
dump=$DUMP_DIR/pre-migrate-$commit.dump
[[ ! -e $dump ]] || dump=$DUMP_DIR/pre-migrate-$commit-$(date -u +%Y%m%dT%H%M%SZ).dump
( umask 077; runuser -u postgres -- /usr/lib/postgresql/18/bin/pg_dump --format=custom "$NAME" > "$dump" ) || die 'pre-migration dump failed'
/usr/lib/postgresql/18/bin/pg_restore --list "$dump" > /dev/null || die "pre-migration dump $dump is unreadable"
echo "dump: $dump"
# As the site's service account, with only migration.env, sandboxed like the
# site itself.
systemd-run --quiet --wait --collect --pipe --service-type=exec --unit="$NAME-migrate" \
  -p DynamicUser=yes -p User="$NAME" -p EnvironmentFile="$MIGRATION_ENV" -p WorkingDirectory="$release" \
  -p NoNewPrivileges=yes -p ProtectSystem=strict -p ProtectHome=yes -p PrivateTmp=yes \
  -p PrivateDevices=yes -p 'RestrictAddressFamilies=AF_UNIX AF_INET AF_INET6' \
  "$NODE" "$release/build/migrate/migrate.js" drizzle || die 'migration failed; the previous release is still serving'
expected=$("$NODE" -e 'console.log(JSON.parse(require("fs").readFileSync(process.argv[1], "utf8")).entries.length)' "$release/drizzle/meta/_journal.json")
applied=$(runuser -u postgres -- psql -X -At -d "$NAME" -c 'select count(*) from drizzle.__drizzle_migrations')
[[ $applied == "$expected" ]] || die "drizzle.__drizzle_migrations has $applied rows, the release has $expected migrations"
echo "migrations: $applied of $expected"

# --- 4. Candidate: the new release under the new unit, on a spare port. -----------
current_step='4 candidate'
step "$current_step"
candidate_port=
for ((port = CANDIDATE_PORTS[0]; port <= CANDIDATE_PORTS[1]; port++)); do
  ss -Hltn "sport = :$port" | grep -q . || { candidate_port=$port; break; }
done
[[ -n $candidate_port ]] || die 'no free candidate port in 4400-4499'
candidate_unit=/run/systemd/system/$CANDIDATE
stop_candidate() {
  systemctl stop "$CANDIDATE" 2>/dev/null || true
  rm -f "$candidate_unit"
  systemctl daemon-reload
}
fail_candidate() {
  journalctl -u "$CANDIDATE" -n 30 --no-pager >&2 || true
  stop_candidate
  die "candidate: $*"
}
grep -q "PORT=$PORT\b" "$release/deploy/$SERVICE" || die "unit does not set PORT=$PORT"
sed -e "s|$SRV/current|$release|g" -e "s|PORT=$PORT\b|PORT=$candidate_port|" \
  "$release/deploy/$SERVICE" > "$candidate_unit"
systemctl daemon-reload
systemctl start "$CANDIDATE" || fail_candidate 'did not start'
echo "candidate on 127.0.0.1:$candidate_port"
( check_health "http://127.0.0.1:$candidate_port" "$commit" ) || fail_candidate '/healthz check failed'
( smoke_app "http://127.0.0.1:$candidate_port" ) || fail_candidate 'smoke check failed'
[[ $(systemctl show -p ExecMainStatus --value "$CANDIDATE") == 0 ]] || fail_candidate 'exited'
stop_candidate
echo 'candidate passed'

# --- 5. Switch and restart. -------------------------------------------------------
current_step='5 switch'
step "$current_step"
if ! cmp -s "$release/deploy/$SERVICE" "$UNIT"; then
  install -m 0644 -o root -g root "$release/deploy/$SERVICE" "$UNIT"
  systemd-analyze verify "$UNIT"
  systemctl daemon-reload
  echo "installed $UNIT"
fi
systemctl enable --quiet "$SERVICE"
ln -sfn "$release" "$SRV/current.next"
mv -Tf "$SRV/current.next" "$SRV/current"
systemctl restart "$SERVICE"

# --- 6. Verify what is live, from the running process. -------------------------------
current_step='6 verify'
step "$current_step"
check_health "http://127.0.0.1:$PORT" "$commit"
smoke_app "http://127.0.0.1:$PORT"
pid=$(systemctl show -p MainPID --value "$SERVICE")
[[ $(readlink "/proc/$pid/exe") == "/opt/node-v$NODE_VERSION-linux-x64/bin/node" ]] || die "$SERVICE runs $(readlink "/proc/$pid/exe")"
[[ $(readlink "/proc/$pid/cwd") == "$release" ]] || die "$SERVICE runs from $(readlink "/proc/$pid/cwd")"
[[ $(head -n 1 "/proc/$pid/cwd/REVISION") == "$commit" ]] || die "$SERVICE reports another revision"
if [[ -x /home/sawyer/github/ubuntu-vm-backup/bin/web-live ]]; then
  /home/sawyer/github/ubuntu-vm-backup/bin/web-live "$SERVICE"
fi

# --- 7. Caddy, only if the site's fragment changed. ---------------------------------
current_step='7 caddy'
step "$current_step"
if cmp -s "$release/deploy/$FRAGMENT.caddy" "$SITE"; then
  echo 'fragment unchanged'
else
  /usr/local/sbin/caddy-apply "$FRAGMENT" "$release/deploy/$FRAGMENT.caddy"
fi

# --- 8. Record, and check what the public sees. --------------------------------------
current_step='8 public check'
step "$current_step"
smoke_public "$commit"
[[ $(preserved) == "$others_before" ]] || die 'another site or a shared Caddy file changed during cutover'
logger -t web-deploy "$HOST $commit live"
trap - EXIT
printf '\nDeployed %s: %s\n' "$commit" "$release"
