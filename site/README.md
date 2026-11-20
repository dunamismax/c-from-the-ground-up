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
npm run dev
```

## Commands

| Command                           | What it does                                                                                     |
| --------------------------------- | ------------------------------------------------------------------------------------------------ |
| `npm run dev`                     | Vite dev server                                                                                  |
| `npm run build`                   | production build into `build/`                                                                   |
| `npm run start`                   | run the production build (`node build`), loading `.env` if present                               |
| `npm run preview`                 | Vite preview of the production build                                                             |
| `npm run check`                   | svelte-check (types)                                                                             |
| `npm run lint` / `npm run format` | Prettier and ESLint                                                                              |
| `npm test`                        | Vitest unit and component tests                                                                  |
| `npm run test:e2e`                | Playwright against `node build` (run `npx playwright install chromium` once; needs the database) |
| `npm run db:start` / `db:stop`    | start or stop the compose database                                                               |
| `npm run db:generate`             | generate a migration from `src/lib/server/db/schema.ts` into `drizzle/`                          |
| `npm run db:migrate`              | apply migrations                                                                                 |
| `npm run db:studio`               | Drizzle Studio                                                                                   |

`GET /healthz` returns 200 with `{"status":"ok","database":"ok"}` when the app
is up and the database answers `select 1`, and 503 otherwise.
