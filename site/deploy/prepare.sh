#!/usr/bin/env bash
# Build and test origin/main as the unprivileged checkout owner, outside the
# deploy lock (step 1 of the deploy flow, see README.md). Prints the exact
# cutover command.
#
#   site/deploy/prepare.sh
#
# Run it from a checkout of the repository on the server (for example
# ~/github/c-from-the-ground-up) whose HEAD is origin/main. It never touches
# anything live. The end-to-end tests need the cfromthegroundup_test database:
# its credentials are in ~/.config/cfromthegroundup/test.env (README.md).
set -Eeuo pipefail
repo=$(cd "$(dirname "$0")/../.." && pwd)
# shellcheck source=site/deploy/common.sh
source "$repo/site/deploy/common.sh"
[[ $EUID != 0 ]] || die 'prepare as the checkout owner, not root'
[[ -z $(git -C "$repo" status --porcelain) ]] || die 'commit all source changes first'
git -C "$repo" fetch --quiet origin
commit=$(git -C "$repo" rev-parse HEAD)
[[ $commit == "$(git -C "$repo" rev-parse origin/main)" ]] || die "$commit is not origin/main; pull or push first"
test_env=${CFTGU_TEST_ENV:-$HOME/.config/cfromthegroundup/test.env}
[[ -f $test_env ]] || die "missing $test_env (the cfromthegroundup_test credentials; see README.md)"
candidate=$HOME/web-work/$HOST-$commit
[[ ! -e $candidate ]] || die "$candidate exists; remove it to prepare again"
mkdir -p "$candidate"

step "Build and test $commit in $candidate"
# The whole repository, not just site/: the site reads the lessons and chapters
# at build time.
mkdir -p "$candidate/src"
git -C "$repo" archive "$commit" | tar -x -C "$candidate/src"
cd "$candidate/src/site"
[[ $(cat .node-version) == "$NODE_VERSION" ]] || die ".node-version is not $NODE_VERSION"
node-26 pnpm install --frozen-lockfile
node-26 pnpm run check
node-26 pnpm run lint
node-26 pnpm run test
# The end-to-end suite against the real test database: the bundled migrator
# as the migration role, then the built server as the web role, with accounts
# on (Better Auth). It builds the site itself.
( set -a; source "$test_env"; set +a; CI=1 node-26 pnpm run test:e2e )
rm -rf build
node-26 pnpm run build
# Caddy serves this file publicly; the live check compares it too.
printf '%s\n' "$commit" > build/client/version.txt

# Smoke test: the build runs on its own and serves the home page. /healthz must
# be 503 here because this throwaway database URL points nowhere.
smoke=$(mktemp -d)
cp -a build package.json "$smoke/"
smoke_port=$(( 20000 + RANDOM % 20000 ))
( cd "$smoke" && exec env HOST=127.0.0.1 PORT=$smoke_port DATABASE_URL=postgres://smoke@127.0.0.1:1/none "$NODE" build/index.js ) &
smoke_pid=$!
trap 'kill "$smoke_pid" 2>/dev/null || true; rm -rf "$smoke"' EXIT
smoke_home=$(get "http://127.0.0.1:$smoke_port/") || die 'smoke: home page did not render'
[[ $smoke_home == *'C From The Ground Up'* ]] || die 'smoke: home page lacks the site title'
[[ $(status_of "http://127.0.0.1:$smoke_port/healthz") == 503 ]] || die 'smoke: /healthz did not report the missing database'
kill "$smoke_pid"; wait "$smoke_pid" 2>/dev/null || true
"$NODE" "$smoke/build/migrate/migrate.js" --check drizzle || die 'smoke: the bundled migrator cannot read drizzle/'
trap - EXIT; rm -rf "$smoke"

# Production dependencies only, from a clean install: `--prod` on top of the
# full install would leave the devDependencies in node_modules/.pnpm. Every
# runtime dependency is bundled into build/ (they are all devDependencies),
# so this is normally empty.
rm -rf node_modules
node-26 pnpm install --frozen-lockfile --prod

step 'Assemble the release'
# The release layout from the contract: REVISION, .node-version, package.json
# ("type": "module" for build/*.js), the build, production node_modules,
# deploy/ (unit and Caddy fragment) and the migrations.
release=$candidate/$RELEASE_DIR
mkdir -p "$release"
printf '%s\n' "$commit" > "$release/REVISION"
cp -a .node-version package.json build node_modules deploy drizzle "$release/"
[[ -z $(find "$release" -type l ! -path "$release/node_modules/*" -print) ]] || die 'release must contain no symlinks outside node_modules'
(cd "$release" && find . -type f -print0 | sort -z | xargs -0 sha256sum) > "$candidate/release.sha256"
printf '%s\n' "$commit" > "$candidate/COMMIT"
printf 'Candidate prepared: %s\nCommit: %s\n' "$candidate" "$commit"
printf 'Deploy (as root): /usr/local/sbin/with-deploy-lock %s/site/deploy/cutover.sh %s\n' "$candidate/src" "$candidate"
