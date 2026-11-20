# AGENTS.md

Guidance for AI agents (and humans) working in `c-from-the-ground-up`. Read this
file before changing anything. Then read [BUILD.md](BUILD.md) for the roadmap,
open decisions, and backlog.

## What this project is

A free, progressive C workbook for beginners, published as an mdBook at
<https://dunamismax.github.io/c-from-the-ground-up/>. It has 35 lessons in five
parts, running from Hello World to sockets, threads, ncurses and a capstone text
adventure. The project is popular, and real learners follow it in order.

Core idea: **every lesson is a small, readable, runnable C program, and the
comments are the lesson.** The book chapters add a prose overview and embed the
full source.

## Current focus: the website

The active phase is building **https://cfromthegroundup.com**, a self-hosted
course site that becomes the main way to take the course. Its stack is
SvelteKit 3, Svelte 5, TypeScript, Tailwind CSS 4, PostgreSQL 18 with Drizzle,
adapter-node and Caddy.

- The full plan, architecture defaults and milestones (W0-W8) are in
  **[BUILD.md](BUILD.md), "Phase W"**. Unless the user asks for something
  else, continue at the first unchecked item of the earliest unfinished
  milestone.
- The site lives in `site/` (see **The website** below). It reads lesson
  content from `Part */` and `src/chapters/` at build time and never keeps its
  own copy.
- SvelteKit 3, Svelte 5 and Tailwind 4 differ a lot from older tutorials.
  Check current docs before writing framework code.

Priorities, in order:

1. **Correct.** Code compiles warning-free, runs, and does what the comments say.
   A wrong comment is a bug.
2. **Clear to a beginner.** Each lesson only uses concepts that earlier lessons
   taught, or explains a new one where it first appears. Prefer plain code over
   clever code.
3. **Safe habits.** Show bounds-checked input, checked return values, and freed
   memory. If a lesson uses an unsafe API for teaching (such as `strcpy` or `atoi`),
   the comments must say why it is risky and what to use instead.
4. **Stable.** People link to lesson files and book pages. Do not rename or move
   files or directories without an explicit decision recorded in BUILD.md.

## Repository rules

- Work on `main` only. Do not create branches, worktrees or pull requests. Commit
  directly to `main` and push with `git push origin main`.
- Do not add AI attribution anywhere: no `Co-Authored-By` trailers, no "Generated
  with ..." lines, and no credit to an AI tool in commits, code, comments or docs.
  Use the configured git identity and never change git config.
- Run `make verify` before every commit, and do not commit if it fails.
- Check `git status` for stray files before committing. Never commit `book/` or
  build outputs; `.gitignore` covers them.
- Commit messages: a short imperative summary line, then a brief body saying what
  changed and why.
- When you finish a BUILD.md item, tick it in the same commit. When you find new
  work you are not doing now, add it to BUILD.md rather than leaving it in chat.

## Layout

```text
Part 1 - The Beginner Path_ Core Concepts/          lessons 1-9   (*.c)
Part 2 - The Intermediate Path_ Deeper into C/      lessons 10-15
Part 3 - The Advanced Path_ Towards Mastery/        lessons 16-25
Part 4 - The Expert Path_ Systems and Concurrency/  lessons 26-30 (POSIX)
Part 5 - Expert Systems & Application Development/  lessons 31-35
    31_make_files_for_multi_file_projects/          the only multi-file lesson (own Makefile)
src/SUMMARY.md                                      book table of contents (mdBook)
src/index.md                                        book introduction page
src/chapters/NN-slug.md                             one chapter per lesson (26 covers server + client)
scripts/sync_chapters.py                            copies lesson sources into chapters; --check for CI
scripts/smoke_check.sh                              compiles every lesson and runs behavioural checks
Makefile                                            maintainer entry points (not a lesson)
book.toml                                           mdBook config; output goes to book/ (git-ignored)
.github/workflows/verify.yml                        CI: listing check, mdbook build, smoke check (clang + gcc), site
.github/workflows/deploy.yml                        publishes the book to GitHub Pages on push to main
assets/images/                                      images (not yet used in the book)
site/                                               the course website, cfromthegroundup.com (see below)
```

The directory names contain spaces, `_` and `&`. **Always quote paths** in shell
commands. Lesson numbers are not zero-padded in `.c` filenames (`1_hello_world.c`),
but chapter files are (`01-hello-world.md`).

## Source of truth and the sync workflow

The `.c`, `.h` and `Makefile` files under `Part */` are the **only** place code
lives. Each chapter embeds them through a marker line followed by a fenced code
block:

````markdown
<!-- source: Part 1 - The Beginner Path_ Core Concepts/1_hello_world.c -->
```c
...verbatim file contents...
```
````

- Never edit code inside a marked block in `src/chapters/`. Edit the lesson file,
  then run `make sync`, which rewrites the block.
- mdBook's `{{#include}}` cannot be used here because it does not support paths
  with spaces. That is why the sync script exists.
- `make check-sync` (run in CI) fails if a listing is stale, a marker points at a
  missing file, a lesson file has no marker or more than one, or a chapter is
  missing from `src/SUMMARY.md`.
- The prose above `## Full Source` in each chapter is hand-maintained. It mirrors
  the lesson's comments but is **not** synced. When you change what a lesson
  teaches, update that prose too.
- Short illustrative snippets in chapter prose (with no marker) are fine and are
  not checked.

## Verification

```sh
make verify              # check-sync + mdbook build + smoke check under clang and $(GCC) + site-verify
make site-verify         # website only: npm ci, check, lint, test, build
make site-e2e            # website Playwright tests (start the database first)
make verify GCC=gcc-15   # point at a real GCC (on macOS, `gcc` is Clang)
make smoke CC=clang      # one compiler only
make serve               # live preview at http://localhost:3000
```

What `scripts/smoke_check.sh` does:

- Compiles every single-file lesson with
  `-Wall -Wextra -Wpedantic -Wstrict-prototypes -Werror` plus `-std=c23`, falling
  back to `-std=c17`. Warnings are errors.
- Adds per-lesson link flags in `compile_lesson()`: `-pthread` (30), `-lm` (32),
  ncurses (33, 35). **A new lesson that needs a library must be added there.**
- Builds lesson 31 through its own Makefile.
- Runs behavioural checks that assert on exact output strings:
  - lesson 30: empty-file handling
  - lesson 26: a server and client exchange over a free local port
  - lesson 17: save/load round trip
  - lesson 29: overlong command rejection
  - lessons 3, 5, 6 and 13: `run_input_checks` feeds valid input, garbage and
    EOF through `run_lesson_with_input`, which enforces a 5-second timeout and a
    1 MiB output cap
  If you change those messages, update the script in the same commit.
- Runs ASan/UBSan regressions when the compiler supports them: lesson 30 on an
  empty file, and a harness that includes lesson 35 with `#define main ...` and
  calls `parse_room()` / `parse_link()` directly. Renaming or changing the
  signatures of those functions, or of `GameState` / `Room`, breaks the harness.

Requirements: a C compiler (Clang and GCC in CI), ncurses headers
(`libncurses-dev` on Debian/Ubuntu), `make`, Python 3.8+, mdBook **0.5.4**,
and Node 24+ with npm for `site-verify`. The site's e2e tests also need Docker
(or any PostgreSQL 18) and a Playwright Chromium (`npx playwright install
chromium` in `site/`).
CI pins mdBook in both workflows. Bump the pin deliberately, in both files, after
`make verify` passes locally with the new version.

## The website (`site/`)

SvelteKit 3 app for https://cfromthegroundup.com. The roadmap is BUILD.md,
Phase W; `site/README.md` has the developer quick start.

```text
site/package.json                  scripts, engines (Node >= 24), `imports` for the #lib alias
site/.nvmrc                        Node 24 (the production and CI version)
site/vite.config.ts                ALL SvelteKit config: the sveltekit({...}) plugin call (incl. the CSP), plus Vitest projects
site/src/env.ts                    environment variables (defineEnvVars), validated at build and start
site/src/app.css                   Tailwind 4 entry: @import "tailwindcss", @plugin typography, @theme tokens
site/src/app.html                  page shell
site/src/routes/                   pages and endpoints (+page.svelte, +server.ts, ...); all prerendered except /healthz
site/src/routes/lessons/[slug]/    lesson page; files/[name] serves each lesson file as a download
site/src/routes/*.e2e.ts           Playwright tests, including a11y.e2e.ts (axe-core, WCAG 2.2 AA, both themes)
site/src/lib/components/           Svelte components (CodeBlock, RunPanel, CourseSidebar, ...)
site/src/lib/site.ts               site-wide constants (name, URLs)
site/src/lib/progress/             learner progress: snapshot logic, ProgressBackend (browser), account-backend.ts (synced), api.ts
site/src/lib/account/              account.svelte.ts: the browser's account state, sign-in/out and delete (fetch, no auth client)
site/src/lib/search.ts             search index options shared by /search-index.json and SearchDialog.svelte
site/src/lib/compiler-explorer.ts  "Try it online" (Compiler Explorer client state and shortener call)
site/src/routes/downloads/         prerendered zip downloads (per lesson and the whole course)
site/src/routes/healthz/+server.ts health probe: 200 when the database answers `select 1`, else 503
site/src/routes/healthz/address/   the client address the app sees (getClientAddress); cutover checks it through Caddy
site/src/routes/api/               dynamic JSON endpoints: account (state, export, delete) and progress
site/src/routes/account/, privacy/ account page (prerendered shell, fills in from /api/account) and privacy policy
site/src/hooks.server.ts           Better Auth under /api/auth/, session lookup and write rate limit for /api/
site/src/lib/                      shared code, imported as #lib/...
site/src/lib/server/               server-only code (never shipped to the browser)
site/src/lib/server/db/            Drizzle: schema.ts (tables, schema `app`) and index.ts (getDb(), lazy postgres.js client)
site/src/lib/server/auth*.ts       Better Auth: auth-options.ts (shared with e2e), auth.ts (getAuth(), getSessionUser())
site/src/lib/server/progress*.ts   progress in the database (applyOps, mergeProgress) and request validation (valibot)
site/src/lib/server/content/       build-time content loader: SUMMARY.md + chapters + lesson files -> Lesson objects
site/src/lib/server/content/metadata.ts  per-lesson link flags, platform, example args; generates compile commands
site/src/lib/content/types.ts      Course / Lesson / Segment types shared with pages and components
site/drizzle/                      generated migrations (committed)
site/scripts/migrate.ts            the migrator; build-migrator.ts bundles it into build/migrate/migrate.js
site/e2e/accounts.ts               e2e account settings and signIn() (Better Auth test helpers, no GitHub)
site/compose.yaml                  local PostgreSQL 18 on 127.0.0.1:5434
site/.env.example                  copy to site/.env (git-ignored) for local development
site/playwright.config.ts          e2e tests run against `node build` (after the migrator), not the dev server
site/deploy/                       production: Caddy site, systemd unit, provision/prepare/cutover/rollback, runbook
```

Commands (run in `site/`):

```sh
npm run dev                 # dev server
npm run build               # production build into build/, plus build/migrate/migrate.js
npm run start               # node build, loading .env if present
npm run check               # svelte-check
npm run lint                # prettier --check + eslint
npm run format              # prettier --write
npm test                    # vitest: *.spec.ts (node) and *.svelte.spec.ts (browser)
npm run test:e2e            # playwright: *.e2e.ts against node build on :4173
npm run db:start            # docker compose up --wait (db:stop to stop)
npm run db:generate         # migration from schema.ts into drizzle/
npm run db:migrate          # apply drizzle/ with scripts/migrate.ts (what production runs)
```

Conventions:

- **SvelteKit 3, not 2.** There is no `svelte.config.js`; options go
  directly in `sveltekit({...})` in `vite.config.ts`, not under `kit`. The
  `$lib` alias is gone: import `#lib/...` **with the file extension**, for
  example `import { getDb } from '#lib/server/db/index.js'` (`.js` resolves to
  the `.ts` file). Environment variables are declared in `src/env.ts` and
  imported from `$app/env/private` or `$app/env/public`, not `$env/*`.
- **Svelte 5 runes only:** `$props`, `$state`, `$derived`, `$effect` and
  snippets (`{@render ...}`). No `export let`, no `$:` and no legacy stores.
  Runes mode is forced for project files in `vite.config.ts`.
- **Tailwind 4 is configured in CSS** (`src/app.css`). Put design tokens in an
  `@theme` block there. Do not add `tailwind.config.js`. Colors are `--c-*`
  properties defined once per theme (`:root` and `[data-theme='dark']`) and
  exposed through `@theme inline`, so use the token utilities (`bg-canvas`,
  `text-muted`, `border-line`, `text-accent`, ...) rather than raw colors or
  `dark:` variants. The theme is `data-theme` on `<html>`, set before paint by
  the script in `app.html`. Code blocks are dark in both themes.
- **Learner state in the browser.** Progress goes through the `progress`
  store (`#lib/progress/progress.svelte.js`); it is empty during prerendering
  and fills in after hydration, so render progress UI only when
  `progress.ready`. Inside an `$effect`, wrap progress writes in `untrack`.
  Tests that press keys right after loading must wait for
  `html[data-hydrated]`.
- **Accounts are optional** (GitHub sign-in through Better Auth, W6). They are
  on only when all four `BETTER_AUTH_*` / `GITHUB_*` variables are set; a
  partial set fails at startup. Every page must work signed out. Pages are
  prerendered, so account state comes from `GET /api/account` after load
  (`account.status`). For a signed-in learner `accountBackend` makes the
  database the record and localStorage a cache; see its header comment for
  how browser progress is merged at sign-in. API endpoints live under
  `/api/`, set `prerender = false`, use `requireUser()` / `readBody()` from
  `#lib/server/api.js`, and validate bodies with valibot in server-only
  modules (keep validators out of the browser bundle). `npm run start` needs
  `ORIGIN`, as production sets it: without it adapter-node assumes https and
  Better Auth does not recognise its own URLs.
- **Content Security Policy** lives in `csp` in `vite.config.ts`
  (`csp.e2e.ts` fails on violations). A new third-party origin the browser
  talks to needs a directive there; an inline `<script>` in `app.html` is
  hashed automatically.
- **Accessibility is tested.** `a11y.e2e.ts` fails on any axe-core WCAG 2.2 AA
  violation. Keep text at AA contrast (do not dim text with `opacity`), give
  every scrollable `pre` `tabindex="0"`, and keep focus rings visible.
- **Drizzle:** tables go in `src/lib/server/db/schema.ts`, in `pgSchema('app')`
  (production's web role may only use schema `app`). Change the schema, run
  `npm run db:generate`, review and commit the SQL under `drizzle/`. Do not use
  `drizzle-kit push` or `drizzle-kit migrate`; `npm run db:migrate` and
  production both use `scripts/migrate.ts`, which keeps its bookkeeping in
  schema `app`. Migrations are forward-only and run while the previous release
  is still live, so a migration must not break it: add first, drop or rename a
  release later. The client is postgres.js (`postgres`). Get it through
  `getDb()`, which connects lazily so builds never need a database.
- **Database holds learner state only.** Course content comes from the lesson
  and chapter files at build time; never copy it into `site/`.
- **Content loader:** pages get content through `getCourse()` / `getLesson()`
  from `#lib/server/content/index.js`, only in prerendered routes (the
  production server has no repository to read). A chapter body arrives as
  ordered `segments`: HTML, a source listing (index into `sources`), or the
  compile-and-run section (render `commands` from the metadata, plus
  `runNotesHtml`). A new lesson that needs link flags, arguments or stdin
  needs an entry in `metadata.ts`, and `metadata.spec.ts` fails if its flags
  disagree with `compile_lesson()` in the smoke check.
- **Formatting:** Prettier (tabs, single quotes, width 100) and ESLint with
  type-checked typescript-eslint rules. Run `npm run format` before committing;
  `npm run lint` must pass.
- **Tests:** unit tests sit next to the code as `*.spec.ts`; component tests are
  `*.svelte.spec.ts` (Vitest browser mode, Chromium); e2e tests are `*.e2e.ts`.
  The e2e server runs with accounts on and test-only settings; sign a browser
  context in with `createLearner()` and `signIn()` from `e2e/accounts.ts`.
  Keep logic in plain modules (like `src/lib/server/health.ts`) so it can be unit
  tested without a server or database.
- **Secrets:** `site/.env` is git-ignored; only `site/.env.example` is
  committed. Production configuration lives on the server
  (`/etc/web-apps/cfromthegroundup/*.env`, see the runbook).
- **Deploying:** production is `ubuntu-vm`, shared with other sites.
  Follow `site/deploy/README.md` exactly (prepare as `sawyer`, then cutover as
  root under `with-deploy-lock`). Deploy only when the maintainer asks, and
  never edit another site's files or the shared Caddyfile.

## Lesson conventions

Follow the existing lessons exactly. `20_linked_lists.c` is a good reference.

- **Header block:**
  ```c
  /**
   * @file 20_linked_lists.c
   * @brief Part 3, Lesson 20: Linked Lists
   * @author dunamismax
   * @date 06-15-2025
   *
   * One-paragraph summary of what the file teaches.
   */
  ```
- Then a `- LESSON START -` banner comment, the lesson body (long explanatory
  block comments interleaved with code), and a `- LESSON END -` banner. After that
  come key takeaways and a `HOW TO COMPILE AND RUN THIS CODE:` section.
- Style: 4-space indentation, Allman braces (opening brace on its own line), and
  `int main(void)` or `int main(int argc, char *argv[])`. Use `snake_case`
  functions and `PascalCase` typedef'd structs. Comments use UPPERCASE for the key
  term being introduced.
- Source files are **ASCII only**: write `--`, not em-dashes, and spell out
  symbols such as "Pi". Use LF line endings with a trailing newline (see
  `.editorconfig`).
- Each lesson compiles cleanly as one translation unit with the smoke-check flags
  under both Clang and GCC. Lessons 26-30 may use POSIX; everything earlier must
  stay ISO C plus the standard library.
- Interactive lessons must not hang when stdin hits EOF or the user types
  garbage. Always check `scanf` (`!= 1`) and `fgets` (`== NULL`) return values,
  and initialize variables that `scanf` fills. Lessons 3 and 6 teach the
  pattern; later lessons should follow it without re-explaining it.
- Free what you allocate. Lessons should be clean under ASan and LeakSanitizer.
- Compile commands inside lessons and chapters currently say `-std=c11`, while
  CI uses C23. This is a known inconsistency with an open decision in BUILD.md.
  Do not mass-edit it until that decision is made.

## Chapter conventions

Each `src/chapters/NN-slug.md` contains:

1. `# Title` matching the lesson's `@brief` title.
2. Prose overview, mostly drawn from the lesson's comments, using `##` sections.
3. `## Full Source`, then the marker and the synced code block (lesson 26 uses
   `### Client Source` / `### Server Source`; lesson 31 uses one `##` per file).
4. `## How to Compile and Run`, with `sh` code blocks.

## Adding a new lesson

1. Pick the next number and the right part. A new part also needs a new
   `Part N - ...` directory and a `# Part N: ...` heading in `src/SUMMARY.md`.
2. Write `Part N - .../NN_slug.c` following the conventions above.
3. If it needs link flags, add a case in `compile_lesson()` in
   `scripts/smoke_check.sh`. If it needs a new system package, add it to the
   apt-get step in `.github/workflows/verify.yml` and to the README Build Notes.
4. If it has meaningful runtime behaviour, add a `run_*_check` function to the
   smoke script that pipes input in and asserts on output.
5. Create `src/chapters/NN-slug.md` with prose, `## Full Source`, an empty marked
   block (`<!-- source: ... -->` then an opening and closing ```` ```c ```` fence),
   and `## How to Compile and Run`.
6. Add the chapter to `src/SUMMARY.md`.
7. Update the part descriptions in `README.md` and `src/index.md` if the scope of
   a part changed.
8. Run `make sync && make verify`. Tick or add the item in BUILD.md, then commit.

## Things to avoid

- Do not rename `Part */` directories or lesson files on your own. External
  links depend on them, and BUILD.md tracks it as an open decision.
- Do not hand-edit synced code blocks, and do not add `{{#include}}`.
- Do not add third-party dependencies to lessons beyond what is already used
  (libc, libm, pthreads, ncurses). New ones, such as libsodium, are planned in
  BUILD.md and need CI and README updates.
- Do not "modernize" a lesson past what the reader has learned at that point.
  Lesson 5 should not use function pointers.
- Do not weaken the smoke-check flags or delete behavioural checks to make a
  change pass.
