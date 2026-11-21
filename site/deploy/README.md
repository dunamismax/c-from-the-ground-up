# Deploying cfromthegroundup.com

The site runs on `ubuntu-vm` alongside the host's other sites and follows the
same conventions: Caddy (`/etc/caddy/sites/*.caddy`), the shared Node 24 runtime
(`/opt/node-runtime`), the shared PostgreSQL 18, and the host-wide deploy lock
(`/usr/local/sbin/with-deploy-lock`).

| What            | Where                                                                       |
| --------------- | --------------------------------------------------------------------------- |
| Releases        | `/srv/www/cfromthegroundup.com/releases/<commit>`, live one at `current`    |
| App server      | `cfromthegroundup.service`, `node build` on `127.0.0.1:4317`, `DynamicUser` |
| Caddy site      | `/etc/caddy/sites/cfromthegroundup.com.caddy` (automatic HTTPS)             |
| Database        | `cfromthegroundup` (schema `app`); roles `_owner`, `_migration`, `_web`     |
| Credentials     | `/etc/web-apps/cfromthegroundup/{web,migration,accounts}.env` (root only)   |
| Backups         | `/var/backups/cfromthegroundup/`, daily via `cfromthegroundup-backup.timer` |
| Cutover backups | `/root/cfromthegroundup-backup-<stamp>-<commit>/` (each has a rollback)     |
| Logs            | `journalctl -u cfromthegroundup`, `/var/log/caddy/cfromthegroundup-*.log`   |

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

## Every deploy

1. Push the commit to `main`.
2. As `sawyer`, build and test it in a fresh candidate directory. Nothing live
   changes:

   ```sh
   cd ~/github/c-from-the-ground-up && git pull --ff-only && site/deploy/prepare.sh
   ```

3. As root, run the cutover command that `prepare.sh` prints:

   ```sh
   /usr/local/sbin/with-deploy-lock /home/sawyer/web-work/cfromthegroundup-<commit>/site/deploy/cutover.sh /home/sawyer/web-work/cfromthegroundup-<commit>
   ```

Cutover stages the release next to the live one, validates Caddy, takes a
backup and applies new migrations (see below), switches the `current` symlink,
restarts the service, waits for `/healthz`, and checks the live site over HTTPS
(`/version.txt`, the home page, `/healthz`, `/api/account`, the `www` redirect,
and that the app sees the client address Caddy saw rather than a spoofed
`X-Forwarded-For`). It also checks that no other site's files changed. If any
step fails, it restores the previous release by itself.

## Migrations

The schema lives in `site/src/lib/server/db/schema.ts`; `npm run db:generate`
writes a migration into `site/drizzle/`, which is committed. The build bundles
a migrator (`site/scripts/migrate.ts`) into `build/migrate/migrate.js`, so it
runs without `node_modules`; `prepare.sh` checks it can read every migration.

When a release has migrations, cutover first runs the nightly backup script
(a full `pg_dump`), then applies any new migrations as
`cfromthegroundup_migration`, in a sandboxed `systemd-run` unit that reads
`migration.env`. The migrator does `SET ROLE cfromthegroundup_owner`, so new
tables belong to the owner and the web role gets row access through the
default privileges `provision.sh` set. Migrations already applied are skipped.

Migrations are forward-only and run while the old release is still live, and a
rollback restores code, not data. So a migration must work for the release
before it too: add tables and columns first, and drop or rename only in a later
release. To undo a migration, restore the database from the backup cutover
made (`pg_restore` of `database.dump` inside the newest
`/var/backups/cfromthegroundup/cftgu-*.tar.gz`).

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

To turn analytics off, set `UMAMI_WEBSITE_ID` back to `''` and deploy; to
remove Umami, `systemctl disable --now umami` as well. The `umami` database
is not in the nightly backup: losing it loses only statistics.

## Rollback

Cutover prints the exact rollback command. In general:

```sh
/usr/local/sbin/with-deploy-lock /root/cfromthegroundup-backup-<stamp>/tools/rollback.sh /root/cfromthegroundup-backup-<stamp>
```

Releases are never deleted, so the previous one is still on disk.

## Checks

```sh
curl -s https://cfromthegroundup.com/healthz       # {"status":"ok","database":"ok"}
curl -s https://cfromthegroundup.com/healthz/address   # your own IP address
curl -s https://cfromthegroundup.com/version.txt   # the live commit
curl -sI https://cfromthegroundup.com/lessons/01-hello-world/files/1_hello_world.c | grep -i content-type   # text/plain; charset=utf-8
systemctl status cfromthegroundup cfromthegroundup-backup.timer
```
