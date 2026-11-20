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
| Credentials     | `/etc/web-apps/cfromthegroundup/{web,migration}.env` (root only)            |
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

Cutover stages the release next to the live one, validates Caddy, switches the
`current` symlink, restarts the service, waits for `/healthz`, and checks the
live site over HTTPS (`/version.txt`, the home page, `/healthz`, the `www`
redirect). It also checks that no other site's files changed. If any step
fails, it restores the previous state by itself.

## Rollback

Cutover prints the exact rollback command. In general:

```sh
/usr/local/sbin/with-deploy-lock /root/cfromthegroundup-backup-<stamp>/tools/rollback.sh /root/cfromthegroundup-backup-<stamp>
```

Releases are never deleted, so the previous one is still on disk.

## Checks

```sh
curl -s https://cfromthegroundup.com/healthz       # {"status":"ok","database":"ok"}
curl -s https://cfromthegroundup.com/version.txt   # the live commit
systemctl status cfromthegroundup cfromthegroundup-backup.timer
```

## Not done yet

- Migrations: there are none yet. Before the first one (milestone W6), add a
  bundled migrator to the build and have `cutover.sh` run it as the migration
  role before the switch. Until then, cutover refuses a release that has a
  `drizzle/` directory.
