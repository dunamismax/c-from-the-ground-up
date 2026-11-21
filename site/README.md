# cfromthegroundup.com

The course website for C From The Ground Up. It is built with SvelteKit 3,
Svelte 5, TypeScript, Tailwind CSS 4, PostgreSQL 18 with Drizzle, and
adapter-node.

Course content is not stored here. The site reads the lessons (`../Part */`)
and chapters (`../src/`) at build time. See the repository's `AGENTS.md` for
conventions and `BUILD.md` (Phase W) for the roadmap.

## Develop

Requires Node 26.13.1 (see `.node-version`), pnpm 12.18.0 (`packageManager`
in `package.json`) and a PostgreSQL 18 database. On the server that is the
test database `cfromthegroundup_test` on the host cluster
(`deploy/README.md`, "Test database"); elsewhere any local PostgreSQL 18.
Dependencies are pinned to exact versions (`saveExact` in
`pnpm-workspace.yaml`); add one with `pnpm add --save-exact` and commit
`package.json` and `pnpm-lock.yaml` together.

```sh
cp .env.example .env        # once; point DATABASE_URL at your database
pnpm install
pnpm run db:migrate         # create the tables (schema app)
pnpm run dev
```

## Commands

| Command                             | What it does                                                                                           |
| ----------------------------------- | ------------------------------------------------------------------------------------------------------ |
| `pnpm run dev`                      | Vite dev server                                                                                        |
| `pnpm run build`                    | production build into `build/`, including the bundled migrator `build/migrate/migrate.js`              |
| `pnpm run start`                    | run the production build (`node build`), loading `.env` if present                                     |
| `pnpm run preview`                  | Vite preview of the production build                                                                   |
| `pnpm run check`                    | svelte-check (types)                                                                                   |
| `pnpm run lint` / `pnpm run format` | Prettier and ESLint                                                                                    |
| `pnpm test`                         | Vitest unit and component tests                                                                        |
| `pnpm run test:e2e`                 | Playwright against `node build` (run `pnpm exec playwright install chromium` once; needs the database) |
| `pnpm run db:generate`              | generate a migration from `src/lib/server/db/schema.ts` into `drizzle/`                                |
| `pnpm run db:migrate`               | apply the committed migrations with `scripts/migrate.ts` (the same migrator production runs)           |
| `pnpm run db:studio`                | Drizzle Studio                                                                                         |

## Accounts

Signing in (GitHub, through Better Auth) is optional and off unless
`BETTER_AUTH_SECRET`, `BETTER_AUTH_URL`, `GITHUB_CLIENT_ID` and
`GITHUB_CLIENT_SECRET` are all set; see `.env.example`. Without them the site
keeps progress in the browser only. The end-to-end tests turn accounts on with
test values and sign in through Better Auth's test helpers
(`e2e/accounts.ts`), so they never contact GitHub.

## Probes

`GET /healthz` returns 200 with
`{"status":"ok","revision":"<commit>","database":"ok"}` when the app is up and
the database answers `select 1` through the app's `pg` pool, and 503 otherwise.
`revision` comes from the release's `REVISION` file (`unknown` in development).

`GET /healthz/address` returns `{"address":"..."}`, the client address the app
sees. Behind Caddy it must be the visitor's own address; `cutover.sh` checks it.
