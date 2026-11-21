# AGENTS.md

Guidance for AI agents (and humans) working in `c-from-the-ground-up`. Read this
file before changing anything. Then read [BUILD.md](BUILD.md) for the roadmap,
open decisions, and backlog.

## What this project is

A free, progressive C course for beginners. Learners take it at
<https://cfromthegroundup.com>, and the same lessons are published as an mdBook
mirror at <https://dunamismax.github.io/c-from-the-ground-up/>. It has 38
lessons in six parts, running from Hello World to sockets, threads, ncurses and
a capstone text adventure, then the tools that keep C correct: debugging,
undefined behavior and testing. The project is popular, and real learners follow it
in order.

Core idea: **every lesson is a small, readable, runnable C program, and the
comments are the lesson.** The book chapters add a prose overview and embed the
full source.

## Current focus: the website

The active phase is building **https://cfromthegroundup.com**, a self-hosted
course site that becomes the main way to take the course. Its stack is
SvelteKit 3, Svelte 5, TypeScript, Tailwind CSS 4, PostgreSQL 18 with Drizzle,
adapter-node and Caddy.

- The full plan, architecture defaults and milestones (W0-W8) are in
  **[BUILD.md](BUILD.md), "Phase W"**. BUILD.md's **Now** section names the
  next step and what is waiting on the maintainer.
- The site lives in `site/` (see **The website** below). It reads lesson
  content from `Part */` and `src/chapters/` at build time and never keeps its
  own copy.
- SvelteKit 3, Svelte 5, Tailwind 4, Drizzle and Better Auth change quickly
  and differ a lot from older tutorials. Look up current documentation
  (Context7 or the official docs) before writing framework code, rather than
  relying on memory.

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

## Start and finish a session

BUILD.md's **How to work this file** section is the full procedure. In short:

1. **Orient.** Read **Now** in BUILD.md, then run `git status --short` and
   `git log -5 --oneline`. Other sessions may work in this checkout at the
   same time and leave uncommitted changes. Leave changes you did not make
   alone.
2. **Choose.** The maintainer's request in chat comes first. Otherwise take
   BUILD.md's **Next step**. When sessions run in parallel, claim the item
   under **In progress** before you start.
3. **Build and verify.** Check the change through its real entry point, not
   only the tests:
   - a lesson: compile it with the smoke-check flags and run it, with valid
     input, garbage and EOF if it reads input
   - a chapter: `make sync`, then read the page in `make serve` or on the site
   - the site: open the page in a browser against `node build`, in both themes
     and at phone width when the layout changes
   - a deploy: the live URL, with the checks under "Checks" in
     `site/deploy/README.md`
   When you fix a bug, first add a check that fails without the fix (the smoke
   check's regressions were each shown to fail against the old lesson). Then
   run `make verify`.
4. **Record.** Tick the BUILD.md item in the same commit, with a sentence on
   how it was verified, or leave it unchecked with a `Remaining:` line. Update
   **Now**, and add a Done log entry.
5. **Finish.** Review `git diff` and `git status`, new files included. Commit
   only your own paths (`git commit -- <paths>`, never `git add -A` when
   another session's work is in the tree), then push `main`. If another
   session's half-finished change breaks `make verify`, check yours in a
   scratch clone of `main` with your changes applied instead.

## What you decide, and what needs the maintainer

Decide it yourself, then record anything a later session would wonder about
in BUILD.md's Decision log:

- code structure, scripts, tests, CI and tooling, within the dependencies
  already in use
- fixes to lessons and chapter prose that follow the lesson conventions below
- splitting, adding or rewording unchecked BUILD.md items when real work calls
  for it
- an open decision that has a stated default: build with the default and say so

Ask the maintainer in chat, and do not act until they answer:

- new product decisions (D11 onward) and changes to settled ones (D1-D10 in
  BUILD.md's Decision log)
- renaming or moving lesson files, `Part */` directories, chapter files or
  published URLs
- a new lesson dependency, or a change to the site's stack
- deploying to production, changing GitHub repository settings, creating
  accounts or OAuth apps, and anything that costs money
- deleting unfinished BUILD.md items, lowering a "Done when", or weakening a
  check (smoke-check flags, behavioural checks, `.clang-tidy`, sanitizers)

An item that needs a person, such as the OAuth app or a screen-reader pass,
never blocks other work. Do everything up to that step, write the exact
remaining instruction under **Waiting on the maintainer** in BUILD.md's Now,
and move on. Never tick a box that is not really done.

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
Part 6 - The Toolsmith Path_ Debugging, UB and Testing/  lessons 36-38
tests/                                              unit tests for lessons 20 and 28 (lesson 38's `#define main` trick); the smoke and sanitizer checks run them
exercises/NN-slug/                                  worked solutions to a chapter's exercises, one `.c` file each (Part 1 so far); the site shows them on the lesson page
fuzz/                                               libFuzzer targets for the lesson 34 and 35 parsers, with seed inputs in fuzz/corpus/<target>/
src/SUMMARY.md                                      book table of contents (mdBook)
src/index.md                                        book introduction page
src/setup.md, src/glossary.md                       book setup chapter and glossary (the site renders the glossary at /glossary)
src/chapters/NN-slug.md                             one chapter per lesson (26 covers server + client)
scripts/sync_chapters.py                            copies lesson sources into chapters; --check for CI, which also checks exercises/ against the chapters
scripts/smoke_check.sh                              compiles every lesson and runs behavioural checks
scripts/check_book_links.py                         checks internal links and anchors in the built book/
scripts/sanitizer_check.sh                          runs the lessons under ASan/UBSan/LSan, MSan and TSan
scripts/pty_run.py                                  runs an ncurses lesson in a pseudo-terminal with scripted keys
scripts/fuzz_check.sh                               builds and runs the fuzz targets (`make fuzz`)
Makefile                                            maintainer entry points (not a lesson)
.clang-format                                       lesson code style; `make format`, checked in CI
.clangd                                             editor flags for the lessons (the smoke check's warnings)
.clang-tidy                                         static-analysis checks, and why some are off; `make tidy`
book.toml                                           mdBook config; output goes to book/ (git-ignored)
book-theme/                                         book-only script and style: a link on every page to the lesson on the site
.github/workflows/verify.yml                        CI: listing and format checks, mdbook build and link check, smoke check (clang + gcc), sanitizers and fuzzing, clang-tidy and GCC analyzer, site
.github/workflows/deploy.yml                        publishes the book to GitHub Pages on push to main (or run by hand)
assets/images/site/                                 screenshots of the site used by README.md
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
  missing from `src/SUMMARY.md`. It also fails if an `exercises/<chapter>/`
  directory does not match its chapter (see "Exercise solutions" below).
- The prose above `## Full Source` in each chapter is hand-maintained. It mirrors
  the lesson's comments but is **not** synced. When you change what a lesson
  teaches, update that prose too.
- Short illustrative snippets in chapter prose (with no marker) are fine and are
  not checked.

## Verification

```sh
make verify              # check-sync + check-format + tidy + mdbook build + book link check + smoke check under clang and $(GCC) + sanitize + fuzz + site-verify
make format              # apply .clang-format to the lessons, exercises/, tests/ and fuzz/, then run make sync
make tidy                # clang-tidy over the lessons and exercises/ (fails on any finding)
make analyze GCC=gcc-16  # GCC -fanalyzer over the lessons and exercises/ (needs real GCC)
make site-verify         # website only: npm ci, check, lint, test, build
make site-e2e            # website Playwright tests (start the database first)
make verify GCC=gcc-15   # point at a real GCC (on macOS, `gcc` is Clang)
make smoke CC=clang      # one compiler only
make smoke-linux         # smoke check under glibc in Docker (GCC and Clang)
make sanitize            # sanitizer check with clang (MSan is skipped on macOS)
make sanitize-linux      # sanitizer check in Docker, MSan included
make fuzz                # fuzz the lesson 34 and 35 parsers (skipped where Clang has no libFuzzer)
make fuzz-linux          # the same in Docker, where LeakSanitizer watches too
make serve               # live preview at http://localhost:3000
```

What `scripts/smoke_check.sh` does:

- Compiles every single-file lesson with
  `-Wall -Wextra -Wpedantic -Wstrict-prototypes -Wshadow -Wconversion
  -Wsign-conversion -Wmissing-prototypes -Werror` plus `-std=c17`, the
  standard the lessons teach (D1). Warnings are errors. When the compiler
  accepts `-std=c23`, it compiles every lesson (lesson 31's files with `-c`)
  a second time under C23.
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
    1 MiB output cap (further arguments become the program's arguments)
  - lessons 15, 16 and the lesson 26 client: `run_number_argument_checks`
    rejects non-numeric, partly numeric and out-of-range arguments
  - lesson 25: `run_text_editor_check` edits and saves a file, rejects bad
    commands and line numbers, and an overlong line
  - lesson 28: `run_hash_table_check`, including the bucket its comment says
    "age" and "city" collide in
  - lesson 34: `run_data_parser_check` parses the sample file from its
    comments, a file with an overlong line, 12 records (it keeps 10) and a
    missing file
  - `exercises/`: `run_exercise_checks` compiles every solution (both
    standards), runs each with no input and with garbage (a hang or a crash
    fails), and asserts on the answer each one prints
  - lessons 36-38: `run_toolsmith_checks` checks their output, builds 36 with
    `-DSHOW_BUG` and 37 with `-DSHOW_UB` and expects the AddressSanitizer and
    UBSan reports the lessons describe, and compiles and runs `tests/*.c`
  If you change those messages, update the script in the same commit.
- Runs ASan/UBSan regressions when the compiler supports them: lesson 30 on an
  empty file, and a harness that includes lesson 35 with `#define main ...` and
  calls `parse_room()` / `parse_link()` directly. Renaming or changing the
  signatures of those functions, or of `GameState` / `Room`, breaks the harness.

What `scripts/sanitizer_check.sh` does (the `sanitizers` CI job, on Linux with
`SANITIZERS_REQUIRED=1`, so nothing may be skipped there):

- Builds every lesson and every `tests/*.c` file with ASan + UBSan (+
  LeakSanitizer where the platform has it) and runs each one in a fresh directory with a canned input from the
  `case` table in `run_lessons()`: valid input, and bad input for the
  interactive and argument-parsing lessons. Lesson 26's server and client run
  as a pair over a local port (`run_socket_pair`). Lessons 33 and 35 run in an
  80x24 pseudo-terminal through `scripts/pty_run.py`, which types their keys
  (`run_terminal_lessons`); a harness also runs 35's `load_world()` and
  `cleanup()` on bad maps and a missing file.
- Builds and runs every solution in `exercises/` the same way, with no input
  and with a line of numbers.
- Repeats the non-terminal runs under MemorySanitizer (Clang on Linux only;
  ncurses is not instrumented) to catch reads of uninitialized memory, and runs
  lesson 30 under ThreadSanitizer.
- Fails on any sanitizer report; every sanitizer exits with status 86, so a
  lesson's own exit status for bad input is not mistaken for a finding.
- **A new lesson that needs input, arguments or files needs a case there**
  (the default runs it with empty stdin and no arguments). A lesson that
  leaks, reads uninitialized memory or races fails this check.

What `scripts/fuzz_check.sh` does (a step of the `sanitizers` CI job, with
`FUZZ_REQUIRED=1`):

- Builds each `fuzz/fuzz_<target>.c` with libFuzzer, ASan and UBSan and runs
  it for `FUZZ_RUNS` inputs (10000 by default), starting from the seed files
  in `fuzz/corpus/<target>/`. `data_parser` feeds bytes to lesson 34's
  `parse_with_sscanf()`; `world_map` loads them with lesson 35's
  `load_world()`, checks every exit points at a loaded room, and calls
  `cleanup()`. Renaming those functions, or `find_room_by_id()`, breaks the
  targets.
- Fails on a crash, a sanitizer report, a leak or a hang, and saves the input
  in `fuzz/artifacts/` (git-ignored; CI uploads it). Replay one with
  `FUZZ_REPRODUCE=<file> sh scripts/fuzz_check.sh <target>`. When a fuzzer
  finds a bug, fix the lesson and add the input to the target's seed corpus
  in the same commit, so it is replayed on every run.
- Runs with `-rss_limit_mb=0`: without it LeakSanitizer sometimes reports
  libFuzzer's own memory-watching thread as a leak at exit (3 runs in 20 on
  macOS). A leak report whose stack shows only `fuzzer::` frames is the
  fuzzer, not the lesson.
- Apple's Clang and GCC have no libFuzzer; the check is skipped there with a
  message. Homebrew's LLVM has it (`make fuzz SANITIZE_CC=/opt/homebrew/opt/llvm/bin/clang`).
- **A new lesson that parses a file or other untrusted input should get a
  target**: `fuzz/fuzz_<name>.c`, seeds in `fuzz/corpus/<name>/`, and its
  name in `TARGETS` in the script.

Requirements: a C compiler (Clang and GCC in CI; Clang for `make sanitize`
and `make fuzz`),
clang-format and clang-tidy 23 (`pipx install clang-format==23.1.5` and
`clang-tidy==23.1.4`, or Homebrew's LLVM),
ncurses headers (`libncurses-dev` on Debian/Ubuntu), `make`, Python 3.8+,
mdBook **0.5.4**, and Node 24+ with npm for `site-verify`. The site's e2e tests also need Docker
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
site/src/routes/og/                prerendered Open Graph images (/og/site.png, /og/<slug>.png; renderer in lib/server/og.ts)
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
  never edit another site's files or the shared Caddyfile. Never change SSH,
  firewall or DNS settings on the host. If a deploy breaks the site, roll it
  back first (`rollback.sh`, see the runbook), then fix it.

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
  `int main(void)` or `int main(int argc, char *argv[])`. `.clang-format`
  encodes the layout: run `make format` then `make sync` after editing a
  lesson; `make check-format` fails otherwise. CI pins clang-format 23. Use `snake_case`
  functions and `PascalCase` typedef'd structs. Comments use UPPERCASE for the key
  term being introduced.
- Source files are **ASCII only**: write `--`, not em-dashes, and spell out
  symbols such as "Pi". Use LF line endings with a trailing newline (see
  `.editorconfig`).
- Each lesson compiles cleanly as one translation unit with the smoke-check flags
  under both Clang and GCC, on macOS and Linux (`make smoke-linux` checks glibc
  from a Mac). Those flags include `-Wmissing-prototypes`, so every function
  other than `main` needs a prototype (lesson 7 teaches them; lessons list them
  under `// --- Function Prototypes ---`) or, from lesson 24 on, can be
  `static`. `-Wconversion` and `-Wsign-conversion` need narrowing conversions
  written as casts; add a comment saying why the value fits.
- Lessons 26-30 may use POSIX; every other lesson must stay ISO C plus the
  standard library (and libm and ncurses where 32, 33 and 35 say so).
- Interactive lessons must not hang when stdin hits EOF or the user types
  garbage. Always check `scanf` (`!= 1`) and `fgets` (`== NULL`) return values,
  and initialize variables that `scanf` fills. Lessons 3 and 6 teach the
  pattern; later lessons should follow it without re-explaining it.
- Free what you allocate. Lessons should be clean under ASan and LeakSanitizer.
- The lessons teach **C17** (D1): every compile command in a lesson, chapter
  or README says `-std=c17`, and so do the site's generated commands
  (`C_STANDARD` in `metadata.ts`). Lessons must also compile under C23, which
  the smoke check tests; a lesson that teaches a C23-only feature must say so
  and give the `-std=c23` command.
- Windows is supported through **WSL** (D3). Run instructions say
  "On Windows (WSL): `./program`", not `program.exe`.
- `@date` is the date the lesson was first published (D5). Leave it alone
  when you edit a lesson; git history records the changes. A new lesson gets
  the date it is added, as `MM-DD-YYYY`.

## Chapter conventions

Each `src/chapters/NN-slug.md` contains:

1. `# Title` matching the lesson's `@brief` title.
2. A callout blockquote: `> **What you'll learn:** ...`, a `>` line, then
   `> **Before you start:** ...` (the lessons it builds on).
3. Prose overview, mostly drawn from the lesson's comments, using `##` sections.
   When the prose talks about a line of code, show it in a short `c` block
   copied from the lesson.
4. `## Full Source`, then the marker and the synced code block (lesson 26 uses
   `### Client Source` / `### Server Source`; lesson 31 uses one `##` per file).
5. `## How to Compile and Run`, with `sh` code blocks.
6. `## Try It Yourself`: three exercises that need only what the course has
   taught so far. A chapter with solutions in `exercises/` ends with the
   paragraph that links to them (copy it from chapter 1).

## Exercise solutions

`exercises/<chapter-slug>/` holds one worked solution per exercise of that
chapter, named `<exercise number>_<snake_case>.c`. Part 1 (chapters 01-09)
has them; BUILD.md's Ideas inbox tracks the rest. `06-loops/3_guessing_game.c`
is a good reference.

- A solution follows the lesson conventions (style, ASCII, strict flags,
  checked input, `-std=c17` in its compile command) and uses **only what the
  course has taught up to that lesson**. Where it needs one thing from the
  next lesson (an `if` in a lesson 4 solution), a comment says which lesson
  covers it.
- The header is `@file`, `@brief Lesson N, Exercise M: ...`, `@author`,
  `@date`, then `THE EXERCISE:` followed by the exercise **word for word**
  from the chapter. `make check-sync` compares the two, so change the chapter
  and the solution together. It also fails when a directory is not named
  after a chapter, an exercise has no solution or two, or the chapter does
  not link to the directory.
- An exercise that asks "what happens?" still gets a `.c` file that compiles:
  the program that shows the behaviour, with the answer in its comments
  (`01-hello-world/3_missing_semicolon.c`, `03-user-input/3_leftover_newline.c`).
- The comments explain why, not just what: the solution is a small lesson.
- Each solution needs lines in `run_exercise_checks` in the smoke check that
  assert on its output. `07-functions/3_definition_order.c` is the one file
  built with `-Wno-missing-prototypes`, because deleting the prototype is the
  exercise.
- The site reads the directory at build time (`Lesson.solutions`) and shows
  each solution closed, under "Solutions" on the lesson page; the zip
  downloads include them. Nothing is copied into chapters or `site/`.

`src/setup.md` (a prefix chapter) and `src/glossary.md` (a suffix chapter,
after `---` in `SUMMARY.md`) are not lessons; the site skips them in
`parseSummary` and renders the glossary at `/glossary`. When a lesson
introduces a new UPPERCASE term, add it to the glossary.

## Adding a new lesson

1. Pick the next number and the right part. A new part also needs a new
   `Part N - ...` directory and a `# Part N: ...` heading in `src/SUMMARY.md`.
2. Write `Part N - .../NN_slug.c` following the conventions above.
3. If it needs link flags, add a case in `compile_lesson()` in
   `scripts/smoke_check.sh`. If it needs a new system package, add it to the
   apt-get step in `.github/workflows/verify.yml` and to the README Build Notes.
4. If it has meaningful runtime behaviour, add a `run_*_check` function to the
   smoke script that pipes input in and asserts on output.
5. Create `src/chapters/NN-slug.md` with the "What you'll learn" box, prose,
   `## Full Source`, an empty marked block (`<!-- source: ... -->` then an
   opening and closing ```` ```c ```` fence), `## How to Compile and Run` and
   `## Try It Yourself`. Add its new terms to `src/glossary.md`.
6. Add the chapter to `src/SUMMARY.md`.
7. Update the part descriptions in `README.md` and `src/index.md` if the scope of
   a part changed.
8. Run `make sync && make verify`. Tick or add the item in BUILD.md, then commit.

## Things to avoid

- Do not rename `Part */` directories or lesson files on your own. External
  links depend on them; D2 settled that they keep their names.
- Do not hand-edit synced code blocks, and do not add `{{#include}}`.
- Do not add third-party dependencies to lessons beyond what is already used
  (libc, libm, pthreads, ncurses). New ones, such as libsodium, are planned in
  BUILD.md and need CI and README updates.
- Do not "modernize" a lesson past what the reader has learned at that point.
  Lesson 5 should not use function pointers.
- Do not weaken the smoke-check flags or delete behavioural checks to make a
  change pass. The same goes for `.clang-tidy`: turn a check off only with a
  comment there saying why it does not fit teaching code, never with NOLINT
  comments in a lesson.

## Pitfalls

Mistakes that have cost real time here:

- **Green on macOS, red on Linux.** glibc hides names that are not ISO C, such
  as `M_PI`, under `-std=c23`, and macOS's `gcc` is Clang under another name.
  Lesson 32 failed CI this way while every Mac run passed. After changing a
  lesson's includes or library calls, run `make smoke-linux`.
- **MemorySanitizer does not run on macOS,** but CI's `sanitizers` job
  requires it. After changing how a lesson allocates, reads or initializes
  memory, run `make sanitize-linux`.
- **`curl | grep -q` under `pipefail`.** `grep -q` exits at the first match,
  curl then fails writing (error 23), and the pipeline fails although the page
  was fine. This rolled back the `46208f2` deploy. Fetch the page in full,
  then match.
- **A loopback port that looks free on a Mac is not free on the server.**
  Umami was first given port 4318, which another site on `ubuntu-vm` uses,
  so the `8a5c018` deploy sent `/stats/` to that site's app until the port
  moved to 4321 (cutover now refuses a port Umami does not own). Check `ss -ltn` and `/etc/caddy/sites`
  on the server before choosing a port, and record it in `common.sh`.
- **`.gitignore` drops fixtures with build-output names.** It excludes `*.map`,
  `*.out`, `*.d`, `*.o`, `*.exe` and more, so `git add` silently leaves out a
  test file with such a name. The world-map fuzz seeds were first named
  `*.map`; the commit lacked them and CI failed. After adding files, check
  `git status --short --ignored <dir>`.
- **Paths with spaces.** Quote every `Part */` path, in scripts and in the
  commands you run.

## Keeping this file current

When a change makes something here wrong (a path, a command, a check, a
convention), fix this file in the same commit. Add to it when you find a
durable convention, a verified command or a recurring pitfall that will help
the next session. Status, results and plan changes go in BUILD.md, not here.
