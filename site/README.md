# cfromthegroundup.com

The course website for C From The Ground Up. It is built with SvelteKit 3,
Svelte 5, TypeScript, Tailwind CSS 4, PostgreSQL 18 with Drizzle, and
adapter-node.

Course content is not stored here. The site reads the lessons (`../Part */`)
and chapters (`../src/`) at build time. See the repository's `AGENTS.md` for
conventions and `BUILD.md` (Phase W) for the roadmap.

## Develop

Requires Node 24 (see `.nvmrc`) and Docker for the local database.

```sh
cp .env.example .env        # once
npm install
npm run db:start            # PostgreSQL 18 on 127.0.0.1:5434
npm run db:migrate          # create the tables (schema app)
npm run dev
```

## Commands

| Command                           | What it does                                                                                     |
| --------------------------------- | ------------------------------------------------------------------------------------------------ |
| `npm run dev`                     | Vite dev server                                                                                  |
| `npm run build`                   | production build into `build/`, including the bundled migrator `build/migrate/migrate.js`        |
| `npm run start`                   | run the production build (`node build`), loading `.env` if present                               |
| `npm run preview`                 | Vite preview of the production build                                                             |
| `npm run check`                   | svelte-check (types)                                                                             |
| `npm run lint` / `npm run format` | Prettier and ESLint                                                                              |
| `npm test`                        | Vitest unit and component tests                                                                  |
| `npm run test:e2e`                | Playwright against `node build` (run `npx playwright install chromium` once; needs the database) |
| `npm run db:start` / `db:stop`    | start or stop the compose database                                                               |
| `npm run db:generate`             | generate a migration from `src/lib/server/db/schema.ts` into `drizzle/`                          |
| `npm run db:migrate`              | apply the committed migrations with `scripts/migrate.ts` (the same migrator production runs)     |
| `npm run db:studio`               | Drizzle Studio                                                                                   |

## Accounts

Signing in (GitHub, through Better Auth) is optional and off unless
`BETTER_AUTH_SECRET`, `BETTER_AUTH_URL`, `GITHUB_CLIENT_ID` and
`GITHUB_CLIENT_SECRET` are all set; see `.env.example`. Without them the site
keeps progress in the browser only. The end-to-end tests turn accounts on with
test values and sign in through Better Auth's test helpers
(`e2e/accounts.ts`), so they never contact GitHub.

## Probes

`GET /healthz` returns 200 with `{"status":"ok","database":"ok"}` when the app
is up and the database answers `select 1`, and 503 otherwise.

`GET /healthz/address` returns `{"address":"..."}`, the client address the app
sees. Behind Caddy it must be the visitor's own address; `cutover.sh` checks it.
