#!/usr/bin/env bash
# Restore the release pointer, site config and unit saved by a cutover, under
# the shared lock:
#   /usr/local/sbin/with-deploy-lock /root/cfromthegroundup-backup-STAMP/tools/rollback.sh /root/cfromthegroundup-backup-STAMP
# Releases are never deleted, so the replaced release is still on disk.
set -Eeuo pipefail
# shellcheck source=site/deploy/common.sh
source "$(dirname "$0")/common.sh"
require_lock
backup=${1:?usage: rollback.sh /root/cfromthegroundup-backup-TIMESTAMP-COMMIT}
[[ $backup == "$BACKUP_PREFIX"* && -f $backup/ready ]] || die 'invalid or incomplete backup directory'

if [[ -f $backup/previous-current ]]; then
  previous=$(cat "$backup/previous-current")
  [[ -d $previous ]] || die "previous release $previous is missing"
  ln -sfn "$previous" "$SRV/current.next"
  mv -Tf "$SRV/current.next" "$SRV/current"
else
  # First deploy: there was no site before it.
  rm -f "$SRV/current" "$SRV/current.next"
fi

if [[ -f $backup/site.service ]]; then start_unit "$backup/site.service"; else remove_unit; fi

if [[ -f $backup/site.caddy ]]; then
  install -m 0644 -o root -g root "$backup/site.caddy" "$SITE"
else
  rm -f "$SITE"
fi

validate_caddy
systemctl reload caddy

if [[ -f $backup/previous-current ]]; then
  printf 'Restored %s; live version: %s\n' "$(readlink "$SRV/current")" "$(local_get /version.txt || echo unreachable)"
else
  echo 'Removed the site (it did not exist before this backup was taken)'
fi
