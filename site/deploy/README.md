# Deploying cfromthegroundup.com

The site runs on `ubuntu-vm` alongside the host's other sites and follows the
host's web platform contract (`docs/web-platform.md` in `ubuntu-vm-backup`):
Caddy (`/etc/caddy/sites/*.caddy`, changed only through `caddy-apply`), Node
26.13.1 at `/opt/node-26` with pnpm 12.18.0 (builds go through the `node-26`
wrapper), the host PostgreSQL 18 cluster, the host-wide deploy lock
(`/usr/local/sbin/with-deploy-lock`), and fix-forward deploys with a candidate
check.

| What                | Where                                                                                                 |
| ------------------- | ----------------------------------------------------------------------------------------------------- |
| Releases            | `/srv/www/cfromthegroundup.com/releases/<commit>`, live one at `current`                              |
| App server          | `cfromthegroundup.service`, `/opt/node-26/bin/node build/index.js` on `127.0.0.1:4317`, `DynamicUser` |
| Caddy site          | `/etc/caddy/sites/cfromthegroundup.com.caddy` (automatic HTTPS)                                       |
| Database            | `cfromthegroundup` (schema `app`, bookkeeping in `drizzle`); roles `_owner`, `_migration`, `_web`     |
| Test database       | `cfromthegroundup_test` (same roles); `~/.config/cfromthegroundup/test.env` for `sawyer`              |
| Credentials         | `/etc/web-apps/cfromthegroundup/{web,migration,accounts}.env` (root only)                             |
| Backups             | `/var/backups/cfromthegroundup/`, daily via `cfromthegroundup-backup.timer`                           |
| Pre-migration dumps | `/var/backups/cfromthegroundup.com/pre-migrate-<commit>.dump` (root only)                             |
| Build work          | `/home/sawyer/web-work/cfromthegroundup.com-<commit>/` (as `sawyer`)                                  |
| Logs                | `journalctl -u cfromthegroundup`, `/var/log/caddy/cfromthegroundup-*.log`                             |

DNS for `cfromthegroundup.com` and `www` points straight at the host (Cloudflare
"DNS only"), so Caddy gets the certificates itself (BUILD.md D10). If the
records are ever switched to proxied, the Caddy file needs the D10 changes
first.

## First time only

As `sawyer`, check out the repository:

```sh
git clone https://github.com/dunamismax/c-from-the-ground-up.git ~/github/c-from-the-ground-up
```

As root, create the database, roles, credentials and the backup timer. It is
idempotent and keeps existing passwords:

```sh
/usr/local/sbin/with-deploy-lock /home/sawyer/github/c-from-the-ground-up/site/deploy/provision.sh
```

## Test database

The end-to-end tests (and so `prepare.sh`) need a PostgreSQL 18 database with
the host's role model. On the server that is `cfromthegroundup_test` on the
host cluster, created once by root with the host's provisioning command:

```sh
cd /home/sawyer/github/ubuntu-vm-backup
/usr/local/sbin/with-deploy-lock bin/provision-app-database cfromthegroundup_test
```

`sawyer` reads its credentials from `~/.config/cfromthegroundup/test.env`
(mode 0600): `DATABASE_URL` (the `_web` role, used by the server and the test
helpers), `MIGRATION_DATABASE_URL` and `DB_OWNER_ROLE` (used by the
migrator). Root copies them there from `/etc/web-apps/cfromthegroundup_test/`.
The tests leave their learners in it; it holds nothing else.

## Every deploy

1. Push the commit to `main`.
2. As `sawyer`, build and test it in a fresh work directory. Nothing live
   changes:

   ```sh
   cd ~/github/c-from-the-ground-up && git pull --ff-only && site/deploy/prepare.sh
   ```

   It refuses a dirty tree or a commit that is not `origin/main`, exports the
   commit with `git archive`, runs `pnpm install --frozen-lockfile`, `check`,
   `lint`, `test`, the end-to-end suite against the test database, the build
   and a smoke test of `build/`, drops the devDependencies, and assembles the
   release (`REVISION`, `.node-version`, `package.json`, `build/`,
   `node_modules/`, `deploy/`, `drizzle/`).

3. As root, run the cutover command that `prepare.sh` prints:

   ```sh
   /usr/local/sbin/with-deploy-lock /home/sawyer/web-work/cfromthegroundup.com-<commit>/src/site/deploy/cutover.sh /home/sawyer/web-work/cfromthegroundup.com-<commit>
   ```

Cutover runs every step in one locked process and stops at the first failure,
naming the step:

2. **Stage** the release as `releases/<commit>` (root-owned, read-only for
   everyone else, checked against the prepared checksums).
3. **Migrate:** dump the database to `/var/backups/cfromthegroundup.com/`,
   apply new migrations (below), and check that `drizzle.__drizzle_migrations`
   has one row per entry in the release's `drizzle/meta/_journal.json`.
4. **Candidate:** start a transient copy of the release's own unit file
   (`cfromthegroundup-candidate.service`, same account, sandbox, env files and
   Node) from the release directory on a free port in 4400-4499, then check
   `/healthz` (`{"status":"ok","revision":"<commit>","database":"ok"}`) and
   the smoke check (main pages, a 404, `/api/account`). It is always stopped
   and removed afterwards.
5. **Switch:** install the unit file if it changed, point `current` at the
   release and restart the service.
6. **Verify** `/healthz` and the smoke check on port 4317, and that the
   running process is `/opt/node-v26.13.1-linux-x64/bin/node` in the new
   release directory with its `REVISION` (as `bin/web-live` reports it).
7. **Caddy:** only if the release's fragment differs from the installed one,
   `caddy-apply cfromthegroundup.com <fragment>`.
8. **Public check** over HTTPS through the local Caddy (`/healthz`, the pages,
   `/version.txt`, security headers, the lesson files' `Content-Type`, the
   client address the app sees, the `www` redirect), that no other site's
   files changed, and a `web-deploy` line in the journal.

## Migrations

The schema lives in `site/src/lib/server/db/schema.ts`; `pnpm run db:generate`
writes a migration into `site/drizzle/`, which is committed. The build bundles
a migrator (`site/scripts/migrate.ts`, `pg` and Drizzle's node-postgres
migrator) into `build/migrate/migrate.js`, so it runs without `node_modules`;
`prepare.sh` checks it can read every migration.

Cutover runs it as the site's service account in a sandboxed `systemd-run`
unit that reads only `migration.env`, so it logs in as
`cfromthegroundup_migration`. The migrator does `SET ROLE
cfromthegroundup_owner`, so new tables belong to the owner and the web role
gets row access through the owner's default privileges. Migrations already
applied are skipped. Drizzle's bookkeeping is `drizzle.__drizzle_migrations`;
until November 2026 it was `app.__drizzle_migrations`, and the first run of
the new migrator copied those rows across (the old table is left in place,
unused).

Migrations are forward-only and run while the old release is still live. So a
migration must work for the release before it too: add tables and columns
first, and drop or rename only in a later release. Restoring a pre-migration
dump over the live database is a recovery decision for the maintainer, never
a deploy step.

## Accounts

Accounts (GitHub sign-in, so progress follows a learner between devices) are
off until the site has a GitHub OAuth app. To turn them on:

1. Create an OAuth app under the maintainer's GitHub account (Settings,
   Developer settings, OAuth Apps): homepage `https://cfromthegroundup.com`,
   callback URL `https://cfromthegroundup.com/api/auth/callback/github`.
2. As root, write `/etc/web-apps/cfromthegroundup/accounts.env` (mode 0600)
   with all four settings. A partial file stops the server at startup.

   ```sh
   umask 077
   cat > /etc/web-apps/cfromthegroundup/accounts.env <<EOF
   BETTER_AUTH_SECRET=$(openssl rand -hex 32)
   BETTER_AUTH_URL=https://cfromthegroundup.com
   GITHUB_CLIENT_ID=<client id>
   GITHUB_CLIENT_SECRET=<client secret>
   EOF
   systemctl restart cfromthegroundup
   curl -s https://cfromthegroundup.com/api/account   # {"enabled":true,"user":null}
   ```

Removing the file and restarting turns accounts off again; stored accounts
stay in the database. Changing `BETTER_AUTH_SECRET` signs everyone out and
makes the stored GitHub tokens unreadable (they are never used after sign-in).
The nightly backup includes this file.

## Analytics

Page-view statistics come from a self-hosted [Umami](https://umami.is)
(BUILD.md W9, decision D9): no cookies, no IP addresses stored, and nothing
sent to a third party. It runs as `umami.service` on `127.0.0.1:4321` with
its own `umami` database. The site's Caddy file passes only two of its paths
to the public, `/stats/script.js` (the tracker) and `/stats/api/send` (where
the tracker reports a page view); its dashboard is never public. The site
loads the tracker only when `UMAMI_WEBSITE_ID` in
`site/src/lib/analytics.ts` is set, so it is off until the steps below are
done. If Umami is down, the tracker fails quietly and the site is unaffected.

To set it up (or update it after changing `UMAMI_VERSION`, and `UMAMI_PNPM` to
the pnpm version that release's `package.json` requires, in `install.sh`):

1. As root, from a checkout of the repository on the server:

   ```sh
   /usr/local/sbin/with-deploy-lock site/deploy/umami/install.sh
   ```

   It creates the `umami` user, role, database and
   `/etc/web-apps/umami/umami.env`, builds the pinned Umami release from
   GitHub as the `umami` user, and starts the service. The first build
   creates the login `admin` with the password `umami`.

2. Open the dashboard through an SSH tunnel from your own machine, then go to
   <http://localhost:4321>:

   ```sh
   ssh -N -L 4321:127.0.0.1:4321 root@ubuntu-vm
   ```

3. Sign in as `admin` / `umami` and change the password at once (Settings,
   Profile).
4. Add a website: name `C From The Ground Up`, domain `cfromthegroundup.com`.
   Copy its website ID (a UUID).
5. Put the ID in `UMAMI_WEBSITE_ID` in `site/src/lib/analytics.ts`, commit,
   and deploy as usual. Check that a page view appears in the dashboard, and
   that the tracker answers through Caddy:

   ```sh
   curl -sI https://cfromthegroundup.com/stats/script.js | head -1   # HTTP/2 200
   ```

Umami is the one part of this site still on the host's Node 24 runtime
(`/opt/node-runtime`): `install.sh` builds it with that runtime's corepack and
the pnpm version Umami pins, and Node 26 ships no corepack. Moving it to
`/opt/node-26` changes how it is built and is a host-wide decision for the
maintainer; until then this site's conformance check reports these two files.

To turn analytics off, set `UMAMI_WEBSITE_ID` back to `''` and deploy; to
remove Umami, `systemctl disable --now umami` as well. The `umami` database
is not in the nightly backup: losing it loses only statistics.

## If a deploy fails

There is no rollback (maintainer's decision, 2026-11-21). Cutover stops at the
first failed step and says which. Up to and including the candidate check the
previous release is still serving and nothing needs cleaning up. If the switch
or the live check fails, the new release is live and unhealthy: do not point
`current` at an older release or reinstall an old unit or fragment. Read
`journalctl -u cfromthegroundup`, fix the cause in a new commit, and deploy
that; if the candidate check missed it, extend the check in the same commit.

## Checks

```sh
curl -s https://cfromthegroundup.com/healthz       # {"status":"ok","revision":"<commit>","database":"ok"}
curl -s https://cfromthegroundup.com/healthz/address   # your own IP address
curl -s https://cfromthegroundup.com/version.txt   # the live commit
curl -sI https://cfromthegroundup.com/lessons/01-hello-world/files/1_hello_world.c | grep -i content-type   # text/plain; charset=utf-8
sudo /home/sawyer/github/ubuntu-vm-backup/bin/web-live cfromthegroundup.service   # node, release, revision, state
systemctl status cfromthegroundup cfromthegroundup-backup.timer
```
