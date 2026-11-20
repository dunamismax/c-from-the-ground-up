#!/usr/bin/env bash
# Build and test a clean, pushed commit as the unprivileged checkout owner,
# outside the deploy lock. Prints the exact cutover command.
#
#   site/deploy/prepare.sh [CANDIDATE_DIR]
#
# Run it from a checkout of the repository on the server (for example
# ~/github/c-from-the-ground-up). It never touches anything live.
set -Eeuo pipefail
repo=$(cd "$(dirname "$0")/../.." && pwd)
# shellcheck source=site/deploy/common.sh
source "$repo/site/deploy/common.sh"
[[ $EUID != 0 ]] || die 'prepare as the checkout owner, not root'
[[ -z $(git -C "$repo" status --porcelain) ]] || die 'commit all source changes first'
git -C "$repo" fetch --quiet origin
commit=$(git -C "$repo" rev-parse HEAD)
git -C "$repo" merge-base --is-ancestor "$commit" origin/main || die "$commit is not on origin/main; push it first"
candidate=${1:-$HOME/web-work/$NAME-$commit}
[[ ! -e $candidate ]] || die "$candidate exists; choose a new candidate directory"
mkdir -p "$candidate"

# The whole repository, not just site/: the site reads the lessons and chapters
# at build time.
git -C "$repo" archive "$commit" | tar -x -C "$candidate"
cd "$candidate/site"
node-runtime npm ci --no-audit --no-fund
node-runtime npm run check
node-runtime npm run lint
node-runtime npm test
node-runtime npm run build
# Caddy serves this file; cutover checks the live site reports the new commit.
printf '%s\n' "$commit" > build/client/version.txt

# Smoke test: the build runs on its own (every dependency is bundled, so no
# node_modules) and serves the home page. /healthz must be 503 here because
# this throwaway database URL points nowhere.
smoke=$(mktemp -d)
cp -a build package.json "$smoke/"
smoke_port=$(( 20000 + RANDOM % 20000 ))
( cd "$smoke" && exec env HOST=127.0.0.1 PORT=$smoke_port DATABASE_URL=postgres://smoke@127.0.0.1:1/none "$NODE" build ) &
smoke_pid=$!
trap 'kill "$smoke_pid" 2>/dev/null || true; rm -rf "$smoke"' EXIT
for _ in {1..30}; do curl -fsS --max-time 2 "http://127.0.0.1:$smoke_port/" >/dev/null 2>&1 && break; sleep 0.5; done
curl -fsS --max-time 5 "http://127.0.0.1:$smoke_port/" | grep -q 'C From The Ground Up' || die 'smoke: home page did not render'
[[ $(curl -s -o /dev/null -w '%{http_code}' --max-time 10 "http://127.0.0.1:$smoke_port/healthz") == 503 ]] || die 'smoke: /healthz did not report the missing database'
kill "$smoke_pid"; wait "$smoke_pid" 2>/dev/null || true
# The bundled migrator runs without node_modules too, and reads every migration.
"$NODE" "$smoke/build/migrate/migrate.js" --check drizzle || die 'smoke: the bundled migrator cannot read drizzle/'
trap - EXIT; rm -rf "$smoke"

# The release: the adapter-node build, package.json ("type": "module" for
# build/*.js) and migrations when there are any; plus the host config.
mkdir -p "$candidate/bundle/web" "$candidate/bundle/config"
cp -a build package.json "$candidate/bundle/web/"
if [[ -d drizzle ]]; then cp -a drizzle "$candidate/bundle/web/"; fi
cp deploy/cfromthegroundup.com.caddy deploy/cfromthegroundup.service "$candidate/bundle/config/"
printf '%s\n' "$commit" > "$candidate/bundle/COMMIT"
(cd "$candidate/bundle" && find . -type f -print0 | sort -z | xargs -0 sha256sum) > "$candidate/bundle.sha256"
snapshot > "$candidate/expected-active"
printf 'Candidate verified: %s\nCommit: %s\n' "$candidate" "$commit"
printf 'Deploy (as root): /usr/local/sbin/with-deploy-lock %s/site/deploy/cutover.sh %s\n' "$candidate" "$candidate"
