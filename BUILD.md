# BUILD.md

The roadmap and work tracker for `c-from-the-ground-up`. Read [AGENTS.md](AGENTS.md)
first for conventions and the verification workflow.

## How to use this file

- **Current focus: the website.** The active phase is **Phase W:
  cfromthegroundup.com** below. Unless the user asks for something else, take
  the first unchecked item in the lowest-numbered unfinished milestone (W1,
  then W2, and so on). Work the milestones in order. Each one lists what "done"
  means.
- **Course content track:** the lesson and tooling backlog further down (Now,
  Next, Later) is still valid, but it is secondary to the website. Work it when
  the user asks, or when a website milestone needs it.
- Skip anything that depends on an unresolved item in **Open decisions**. Those
  need the maintainer. Where a decision has a stated **default**, proceed with
  the default and note that you did.
- **Finishing work:** tick the box (`- [x]`) in the same commit as the change. If
  the item is fully done, move it to the **Done log** with the date.
- **New ideas:** when you spot something worth doing but out of scope, add it to
  **Ideas inbox** with one line of context (what, where, why). Do not leave it
  only in chat or a commit message.
- **Decisions:** when the maintainer settles an open decision, move it to the
  **Decision log** with the date and outcome, then turn its consequences into
  backlog items.
- Keep items concrete: name the files, the lessons, and what "done" looks like.

## Current state (2026-11-20)

- 35 lessons in 5 parts (lesson 26 has a server and a client; lesson 31 is
  multi-file). Book published with mdBook 0.5.4 to GitHub Pages.
- `make verify` is green locally (Clang 23, Apple `gcc`) and in CI (Ubuntu Clang
  and GCC). It covers:
  - the listing sync check
  - `mdbook build`
  - a compile of every lesson with `-Wall -Wextra -Wpedantic -Wstrict-prototypes
    -Werror -std=c23`
  - behavioural checks for lessons 3, 5, 6, 13, 17, 26, 29 and 30. The
    interactive lessons are fed valid input, garbage and EOF under a 5-second
    timeout and a 1 MiB output cap.
  - ASan/UBSan regressions for lessons 30 and 35
- Code listings in the book are generated from the `.c` files by
  `scripts/sync_chapters.py`. Chapter prose is still hand-maintained and can drift.
- **Website:** the domain `cfromthegroundup.com` is registered, and its DNS in
  Cloudflare points at the maintainer's self-hosted Ubuntu server. W1 is done:
  `site/` holds a SvelteKit 3 scaffold with a placeholder home page and
  `/healthz`, wired into `make verify` and CI. W2 is done: the content loader
  in `site/src/lib/server/content/` turns the chapters and lesson files into
  typed, highlighted `Lesson` objects. W3 is done except a Lighthouse run: the
  design system, landing, course overview, part, lesson, setup, about and 404
  pages, all prerendered. W4 is done: progress in the browser, search,
  "Try it online" through Compiler Explorer, keyboard shortcuts and zip
  downloads. Next: the open W5 items (CSP, client-IP test, migrator), then
  W6, which needs decision D7.
- **Live:** https://cfromthegroundup.com serves the W1 placeholder from
  `ubuntu-vm` (deployed 2026-11-20 with `site/deploy/`). Most of W5 is done; its
  remaining items are listed there.

---

## Phase W: cfromthegroundup.com (CURRENT FOCUS)

### Vision

`https://cfromthegroundup.com` becomes **the** place to take this course. It
should be fast and beautiful, and it should feel like a real course rather than
a code dump. A learner lands on it, sees the path from Hello World to systems
programming, starts lesson 1, and comes back days later to the exact place they
left off.

The site is the pinnacle of the project. The GitHub repo becomes "the source
code of the course", and the mdBook on GitHub Pages becomes a mirror until the
site reaches parity (see D6).

### Tech stack (fixed by the maintainer)

| Layer | Choice |
| --- | --- |
| Framework | SvelteKit **3** with Svelte **5** (runes) |
| Language | TypeScript (strict) |
| Styling | Tailwind CSS **4** (CSS-first config) |
| Database | PostgreSQL **18** + Drizzle ORM / drizzle-kit |
| Runtime | Node (LTS, 24.x) via `@sveltejs/adapter-node` |
| Edge / TLS | Caddy reverse proxy on the same Ubuntu server, behind Cloudflare DNS |

Do not swap any of these without the maintainer's approval. These tools change
quickly, so **check current documentation** (Context7 or the official docs)
before relying on memory. In particular:

- **SvelteKit 3 is not SvelteKit 2.** Configuration lives in the `sveltekit({...})`
  plugin call in `vite.config.ts`. SvelteKit 3 does not read `svelte.config.js`,
  and options are no longer nested under `kit`.
- SvelteKit 3 needs Node >= 22.17, TypeScript 6, Vite 8, Svelte >= 5.57 and
  `@sveltejs/vite-plugin-svelte` 7.
- CSRF protection is always on. Use `csrf.trustedOrigins` if you need
  cross-origin posts.
- Remote functions (`query` / `form` / `command` / `prerender` in `*.remote.ts`
  files) are available for client-server calls.
- Most tutorials online still show the 2.x layout.

### Architecture (decided defaults: follow them unless the maintainer says otherwise)

- **One repo.** The site lives in `site/` in this repository. The lessons stay in
  `Part */`, and their chapters stay in `src/chapters/`.
- **Content stays in git, not in the database.** The site reads course content
  at build time from:
  - `src/SUMMARY.md` for order, parts and titles
  - `src/chapters/*.md` for prose
  - the lesson source files, through the existing `<!-- source: ... -->` markers
  Never copy lesson text or code into `site/`. The single-source-of-truth rule
  in AGENTS.md applies to the site too.
- **Lesson pages are prerendered.** Course pages are static HTML generated at
  build time (`export const prerender = true`), which keeps them fast and
  cacheable. Only learner-specific features (progress, accounts) hit the Node
  server and the database.
- **Markdown pipeline:** unified (`remark-parse`, `remark-gfm`, `remark-rehype`,
  `rehype-slug`, `rehype-autolink-headings`), with Shiki for syntax highlighting
  (`c`, `makefile`, `sh`) at build time. Ship zero highlighting JavaScript to the
  browser.
- **Stable URLs:**
  - lessons at `/lessons/<chapter-slug>` (for example `/lessons/01-hello-world`,
    matching the chapter file names)
  - parts at `/parts/<n>`
  Once published, URLs never change; add redirects instead.
- **The database holds learner state only:** users, sessions, lesson progress
  and, later, notes and exercise results. Schema in
  `site/src/lib/server/db/schema.ts`. Migrations are generated with
  `drizzle-kit generate`, committed under `site/drizzle/`, and applied at deploy
  time before the new build starts.
- **Svelte 5 only:**
  - runes (`$state`, `$derived`, `$props`, `$effect`) and snippets
  - no `export let`, no `$:` and no legacy stores in new code
- **Tailwind 4 is configured in CSS:** `@import "tailwindcss";` plus `@theme`
  tokens in `site/src/app.css`. Add `@tailwindcss/typography` through `@plugin`
  for prose. There is no `tailwind.config.js`.
- **Package manager:** npm, with a committed `package-lock.json`. Pin the Node
  version in `site/.nvmrc` and in `engines`.
- **Secrets stay out of git.** Commit `site/.env.example`; `site/.env` is ignored.
  Production configuration lives in an environment file on the server (see W5).
- **Deploys go through `site/deploy/`.** Configs, scripts and the runbook
  (`site/deploy/README.md`) live in git. Agents may run them over
  `ssh root@ubuntu-vm` only when the maintainer asks for a deploy.

### Milestones

#### W0. Inputs from the maintainer (agents: ask, do not block on these)

- [x] Server facts (found by inspection, 2026-11-20):
  - `ubuntu-vm`, Ubuntu 26.04.1 LTS, x86_64; reached as `ssh root@ubuntu-vm`
  - Caddy 2.11 (custom build) and PostgreSQL 18.6 were already installed, and
    about ten other sites run on the box, each with its own
    `/etc/caddy/sites/*.caddy`, `/srv/www/<site>/releases`, systemd unit and
    database, deployed under the host-wide lock `with-deploy-lock`
  - app directory `/srv/www/cfromthegroundup.com` (host convention), checkout
    at `/home/sawyer/github/c-from-the-ground-up`
- [x] Cloudflare setup: both records are **DNS only**. They resolve straight to
      the host's public IP, so Caddy's automatic HTTPS applies (D10).
- [ ] Decisions D6-D10 below. Every one has a default, so W1-W5 can proceed
      without them.

#### W1. Scaffold the app

- [x] Create `site/` with `npx sv create site`: SvelteKit minimal template with
      TypeScript.
- [x] Add the add-ons with `npx sv add`:
  - tailwindcss
  - drizzle (PostgreSQL)
  - eslint
  - prettier
  - vitest
  - playwright
  - the node adapter
  Confirm that the result has the SvelteKit 3 layout described above. Record
  the Postgres client choice (`postgres` / postgres.js or `pg` /
  node-postgres) in the Decision log.
- [x] Add a local development database: `site/compose.yaml` running `postgres:18`
      on localhost, with a `DATABASE_URL` example in `site/.env.example`.
- [x] Add npm scripts:
  - `dev`, `build`, `preview`
  - `check` (svelte-check), `lint`, `format`
  - `test` (vitest), `test:e2e` (playwright)
  - `db:generate`, `db:migrate`, `db:studio`
- [x] Add a `/healthz` endpoint that returns 200 when the app is up and can run
      `select 1` against the database. Caddy and the deploy script will use it.
- [x] Update the root `.gitignore`: `site/node_modules/`, `site/.svelte-kit/`,
      `site/build/`, `site/.env`, `site/test-results/`, `site/playwright-report/`.
- [x] Wire the site into the repo gates:
  - a `make site-verify` target that runs `npm ci`, `check`, `lint`, `test` and
    `build` in `site/`
  - `make verify` runs `site-verify` too
  - a `site` job in `.github/workflows/verify.yml` on Node 24, with a
    `postgres:18` service container for tests that need a database
- [x] Update AGENTS.md:
  - the `site/` layout
  - the site commands
  - Svelte, Tailwind and Drizzle conventions
- **Done when:**
  - `make verify` and CI are green
  - `npm run build && node build` serves a placeholder page
  - `/healthz` returns 200 against the local database
- **Status:** done 2026-11-20 (see the Done log).

#### W2. Course content pipeline

- [x] Write a content loader in `site/src/lib/server/content/`. It parses
      `src/SUMMARY.md` into parts and lessons, loads each chapter, resolves the
      source markers and returns typed `Lesson` objects (types in
      `site/src/lib/content/types.ts`) with these fields:
  - number, slug, title and part
  - prose HTML and a table of contents
  - source files, each with path, language and contents
  - compile and run instructions
  - previous and next lesson
- [x] Render the markdown to HTML at build time with the unified and Shiki
      pipeline. Give headings anchors, and render the "Full Source" listings with
      file names. The body comes back as ordered segments (HTML, source listing,
      compile-and-run section) so the lesson page can render listings and the
      run panel as components; each source carries its name, highlighted HTML
      and GitHub links.
- [x] Add a small per-lesson metadata table in code
      (`site/src/lib/server/content/metadata.ts`) covering:
  - required link flags (`-pthread`, `-lm`, ncurses)
  - whether the lesson needs command-line arguments or stdin
  - platform: portable C, POSIX, or ncurses
  The UI uses it for compile commands and badges. `metadata.spec.ts` asserts
  that the flags agree with `compile_lesson()` in `scripts/smoke_check.sh`.
- [x] Generate compile commands from that metadata rather than copying them
      from prose, so resolving D1 later is a one-line change (`C_STANDARD` in
      `metadata.ts`).
- [x] Add Vitest tests that check:
  - every chapter in `SUMMARY.md` yields a lesson
  - slugs are unique and match the chapter file names
  - every embedded source equals the file on disk
  - no lesson has empty prose
- **Done when:** all 35 chapters (36 source pages, counting both files of
  lesson 26) load through the loader with passing tests, and the content changes
  automatically when a `.c` file changes and `make sync` runs.
- **Status:** done 2026-11-20 (see the Done log).

#### W3. Design system and core pages

- [x] Visual identity: a logo or wordmark, a color palette and a type scale.
  - a readable serif or sans for long-form prose, plus a monospace for code
  - light and dark themes, with dark preferred for code
  - define everything as Tailwind `@theme` tokens
  Done: the mark (a C on two layers of ground, `Logo.svelte` and
  `favicon.svg`), warm paper neutrals with a "C blue" accent and an ember
  highlight, system font stacks only (serif prose, sans UI, mono code), all as
  `--c-*` properties per theme exposed through `@theme inline` in `app.css`.
  Code is dark in both themes.
- [x] Layout: a header and footer, plus a course sidebar that lists the parts
      and lessons. The sidebar collapses on mobile. Add skip links.
- [x] Pages:
  - landing (`/`): pitch, curriculum overview, "Start lesson 1" call to action
  - course overview (`/lessons`): all parts, lesson cards, what each part
    covers (part summaries are parsed from the Structure list in
    `src/index.md`)
  - lesson pages (`/lessons/[slug]`)
  - part pages (`/parts/[n]`), per the stable-URL plan
  - Setup guide (`/setup`): installing a compiler on macOS, Linux and Windows
    (WSL), and running your first program. This absorbs the "Setup chapter"
    item from the Later backlog.
  - about (`/about`), with a GitHub link and the license
  - a custom 404 page
- [x] Lesson page components:
  - code block with copy button, line numbers, file name, download link and a
    "View on GitHub" link (downloads are prerendered at
    `/lessons/<slug>/files/<name>`)
  - compile and run panel with macOS / Linux / Windows (WSL) tabs
  - an "on this page" table of contents
  - previous and next navigation
  - part and lesson breadcrumbs
  - platform badges (POSIX, ncurses)
- [x] Accessibility target: WCAG 2.2 AA. Pages must be fully keyboard navigable,
      use visible focus rings, and respect `prefers-reduced-motion`.
      `src/routes/a11y.e2e.ts` runs axe-core (WCAG 2.2 AA rules) on every kind
      of page in both themes; other e2e tests cover the skip link, keyboard tab
      switching and the mobile outline. A manual screen-reader pass belongs to
      the W7 launch check.
- [ ] Performance target: Lighthouse 95+ on mobile for lesson pages. Not yet
      measured. Pages ship no web fonts and little JavaScript; the largest
      lesson page (35) is 20 KB with Brotli.
- **Done when:** every lesson is readable on the site at parity with the mdBook,
  on desktop and mobile, in both themes, and Playwright e2e tests cover the
  landing page, a lesson page and navigation.
- **Status (2026-11-20):** done except the Lighthouse measurement.

#### W4. Learner experience (no accounts yet)

- [x] Progress tracking in `localStorage`, behind a small progress-store
      interface so W6 can swap in the database
      (`site/src/lib/progress/progress.ts`: `ProgressSnapshot` and
      `ProgressBackend`; `progress.svelte.ts` is the reactive store):
  - mark a lesson complete
  - per-part progress bars
  - "Continue where you left off" on the landing page and the course overview
- [x] Site search over lesson titles, headings and prose, using a client-side
      index generated at build time (MiniSearch, `/search-index.json`, 34 KB
      with Brotli, loaded when search first opens). It also indexes each
      source file's comments, since those are the lesson.
- [x] "Try it online" button for portable lessons. It opens the lesson in
      Compiler Explorer (godbolt.org) with the source and flags preloaded and
      execution enabled (GCC 16.2, `cg162`, through Compiler Explorer's
      shortener API, called from the browser only on click). POSIX, ncurses,
      multi-file and file-reading lessons instead say to run locally, with a
      link to `/setup`. Native in-browser execution is D8.
- [x] Keyboard shortcuts: `j` / `k` or arrow keys for previous and next lesson,
      and `/` (or Ctrl/Cmd+K) for search.
- [x] Downloads: each lesson's source files, a zip per lesson
      (`/downloads/<slug>.zip`) and a zip of the whole course
      (`/downloads/c-from-the-ground-up.zip`), built reproducibly at build
      time with fflate.
- **Done when:** a learner can work through the course from start to finish
  with persistent progress in one browser, find any topic by search, and run
  portable lessons without installing anything.
- **Status:** done 2026-11-20 (see the Done log).

#### W5. Production deployment

Everything goes in `site/deploy/`, with a step-by-step `site/deploy/README.md`
runbook.

- [x] `Caddyfile` (`site/deploy/cfromthegroundup.com.caddy`):
  - `cfromthegroundup.com` reverse-proxies to `127.0.0.1:4317` (3000 was the
    plan; 4311-4316 belong to other sites), serves client assets from disk,
    and `www` redirects to the apex
  - `encode zstd gzip`
  - long-lived `Cache-Control` for `/_app/immutable/*`
  - security headers: HSTS, `X-Content-Type-Options`, `Referrer-Policy`
  - a `/healthz` health check (cutover waits on it and checks it through
    Caddy)
  - TLS according to D10 (automatic HTTPS, Let's Encrypt)
- [x] `cfromthegroundup.service`: a systemd unit that runs `node build` as a
      dedicated unprivileged user (`DynamicUser`), with
      `EnvironmentFile=/etc/web-apps/cfromthegroundup/web.env` (host convention),
      `Restart=on-failure` and systemd hardening (`NoNewPrivileges`,
      `ProtectSystem=strict` and so on).
- [ ] Production environment template. Done: `DATABASE_URL` in `web.env`
      (written by `provision.sh`), and `ORIGIN`, `PORT`, `HOST`,
      `ADDRESS_HEADER=X-Forwarded-For` and `XFF_DEPTH=1` in the unit (DNS only,
      so Caddy is the only proxy). Still open: the test below, which needs an
      endpoint that reports the address.
  - `DATABASE_URL`, `ORIGIN=https://cfromthegroundup.com`, `PORT`, `HOST`
  - the adapter-node proxy settings (`PROTOCOL_HEADER`, `HOST_HEADER`,
    `ADDRESS_HEADER`, `XFF_DEPTH`)
  Choose the proxy settings so that `event.getClientAddress()` returns the
  visitor's real IP through Cloudflare and Caddy (Caddy `trusted_proxies`), and
  test it.
- [x] Deploy scripts, run on the server: `prepare.sh` (as `sawyer`) and
      `cutover.sh` (as root under the lock) replace the planned `deploy.sh`,
      following the host's other sites. They build a pushed commit from
      `git archive` rather than `git pull`. Original plan:
  - updates the code (`git pull` on `main`)
  - runs `npm ci` and `npm run build`
  - applies migrations
  - restarts the service and waits for `/healthz`
  - keeps the previous build directory for a quick rollback
- [ ] Migrations at deploy time: there are none yet. Before the first one (W6),
      bundle a migrator into the build (the host's other SvelteKit site ships
      `build/migrate/migrate.js`) and have `cutover.sh` run it as
      `cfromthegroundup_migration` before the switch. Until then, cutover
      refuses a release with a `drizzle/` directory.
- [x] PostgreSQL 18 setup (`site/deploy/provision.sh`; 18 was already
      installed from Ubuntu's archive):
  - install from Ubuntu's archive if it ships 18, otherwise from the PGDG apt
    repository
  - a dedicated database and role, local-only connections
  - a nightly `pg_dump` systemd timer with rotation
    (`cfromthegroundup-backup.timer`, 30 days in `/var/backups/cfromthegroundup`)
- [ ] Use CSP in SvelteKit's `csp` config (nonce- or hash-based) rather than a
      static Caddy header, so inline scripts SvelteKit generates keep working.
- **Done when:** `https://cfromthegroundup.com` serves the site with valid TLS,
  `www` redirects, `/healthz` is green, a redeploy is one command, and a
  database backup exists.
- **Status (2026-11-20):** live. TLS, the `www` redirect, `/healthz` and a
  backup are verified. A redeploy is two commands (prepare, then cutover). Open
  items: CSP, the client-IP test, and the migrator.

#### W6. Accounts and synced progress (needs D7)

- [ ] Authentication with Better Auth (`npx sv add better-auth`) and its Drizzle
      adapter, using the sign-in methods chosen in D7.
- [ ] Add a `lesson_progress` table (user, lesson slug, `completed_at`,
      `last_viewed_at`, unique on user and slug), and have the progress store
      talk to it through remote functions.
- [ ] On first sign-in, merge the progress already in `localStorage` into the
      account.
- [ ] Rate-limit sign-in and other write endpoints.
- [ ] Account page with data export and account deletion.
- [ ] Privacy policy page covering what is stored and why.
- **Done when:** progress follows a signed-in learner across devices, and
  signed-out learning still works exactly as in W4.

#### W7. Launch and make the site the front door

- [ ] SEO:
  - per-page `<title>`, description and canonical URL
  - Open Graph images, generated per lesson at build time
  - `sitemap.xml` and `robots.txt`
  - `Course` / `LearningResource` structured data
- [ ] Point everything at the site:
  - set the repo homepage to `https://cfromthegroundup.com`
  - make the site the first link in `README.md`
  - add a banner to `src/index.md`, and apply D6 to the mdBook
- [ ] Map old mdBook URLs (`/c-from-the-ground-up/chapters/NN-slug.html`) to
      the new lesson URLs, with redirects or links depending on D6.
- [ ] Final launch check: accessibility, performance and broken-link passes on
      the production URL.
- **Done when:** the site is the documented primary way to take the course.

#### W8+. Beyond parity (triage into milestones when W7 is done)

- Exercises per lesson with automated checking (see "Exercises" in Later), and
  short end-of-lesson quizzes.
- In-browser compile and run (D8).
- Completion certificates per part and for the whole course.
- Notes and bookmarks per lesson (needs W6).
- Self-hosted, privacy-friendly analytics (D9).
- Offline support (PWA), and a printable or PDF edition generated from the same
  content.
- Translations, if the audience asks for them.

### Website decisions (maintainer)

- [ ] **D6. Future of the mdBook on GitHub Pages.**
  - Default: keep publishing it until W7, then add a banner on every page
    pointing to the matching lesson on the site.
  - Alternatives: full redirect, or retire it.
- [ ] **D7. Sign-in methods.**
  - Default: GitHub OAuth only. It fits the audience and needs no email service.
  - Options: add email magic links (requires an SMTP or email API provider),
    and/or email + password.
- [ ] **D8. Running C in the browser.**
  - Default: link out to Compiler Explorer (W4).
  - Options:
    - Compiler Explorer's API from our backend
    - a WebAssembly toolchain in the browser (heavy download, sandboxed)
    - our own sandboxed compile server, using nsjail or Firecracker on the
      server. This has a real security cost and must not be built without
      explicit approval.
- [ ] **D9. Analytics.**
  - Default: none until launch.
  - Options: self-hosted Umami or Plausible, sharing the PostgreSQL instance.
- [x] **D10. Cloudflare proxy and TLS.** Settled 2026-11-20: the records are
      DNS only, so Caddy's automatic HTTPS is used (see the Decision log).
  - If the DNS record is **DNS only**, Caddy's automatic HTTPS works
    unchanged.
  - If it is **proxied**, set Cloudflare SSL/TLS to **Full (strict)** and do
    one of the following:
    - (a) Default: install a Cloudflare Origin CA certificate in Caddy with the
      `tls` directive.
    - (b) Build Caddy with the `caddy-dns/cloudflare` module and use the ACME
      DNS challenge, which needs a scoped API token on the server.
  - Either way, configure Caddy `trusted_proxies` with Cloudflare's IP ranges
    so client IPs are correct.

---

## Open decisions: course content (maintainer)

Website decisions D6-D10 are listed under Phase W. These block related backlog
items. Agents: do not act on them; ask the user, or
leave them alone.

- [ ] **D1. Which C standard do the lessons teach?** CI compiles with `-std=c23`
      (falling back to c17), but 113 compile commands in lesson comments and chapter
      "How to Compile and Run" sections say `-std=c11`. Lesson 1 explains the flag
      as "the C11 standard".
  - Options:
    - (a) Teach `-std=c17`. It is universally available and needs no fallback
      note.
    - (b) Teach `-std=c23` with a "use `-std=c2x` or `-std=c17` on older
      compilers" note. Ubuntu 22.04's GCC 11 rejects `-std=c23`.
    - (c) Keep c11 and just say CI checks newer standards too.
  - Whichever is chosen, update every lesson, chapter, README and `src/index.md`
    in one sweep, and teach the chosen flag in lesson 1.
- [ ] **D2. Rename the `Part N - ...` directories to path-safe names?** Examples:
      `part-1-beginner/01_hello_world.c`. Spaces and `&` make every shell command
      awkward and stop mdBook `{{#include}}` from working, which is why the sync
      script exists.
  - Cost: it breaks every external deep link to a lesson file on GitHub, and
    GitHub does not redirect moved paths.
  - If yes: rename with `git mv`, update `scripts/`, `AGENTS.md`, `README.md`,
    the markers and the smoke check, then consider replacing the sync script with
    `{{#include}}`.
- [ ] **D3. Windows support level.** Today it is "use WSL for lessons 26-35". The
      options: add a CI job for MinGW (MSYS2) or MSVC on lessons 1-25, or state
      WSL as the official Windows path.
- [ ] **D4. Which new-content tracks to pursue?** See **Content expansion**. The
      libsodium, fuzzing and CMake tracks add dependencies and CI weight. Pick the
      tracks before anyone starts them.
- [ ] **D5. `@date` headers.** Every lesson says `06-15-2025`. The options:
      leave them, drop the tag, or update it on meaningful edits.

---

## Course content track

Secondary to Phase W. These items still matter: the website is only as good as
the lessons it serves.

### Now (P1): correctness for learners

- [x] **Unchecked `scanf` made interactive lessons misbehave.** Done
      2026-11-20; see the Done log.
- [ ] **EOF and hang sweep for the remaining lessons.** Lessons 3, 5, 6 and 13
      are covered by `run_input_checks` in `scripts/smoke_check.sh`. A manual
      `</dev/null` run of lessons 9, 12, 17, 18 and 29 found no hangs. Sweep
      every other lesson that reads stdin the same way. Lessons that need
      arguments (16, 25, 34, ...) should print usage and exit non-zero, which is
      fine. Add any interactive lesson to `run_input_checks` with
      valid, garbage and EOF cases.
- [ ] **Uninitialized-read detection.** The behavioural checks cannot see a
      read of an uninitialized variable when the garbage value happens to
      produce the right output. The old lesson 13 passed them for exactly that
      reason. Consider a Linux CI job that builds the interactive lessons with
      Clang `-fsanitize=memory` and replays the `run_input_checks` inputs.
- [ ] **Unsafe-API audit.** Check every use of `strcpy`, `atoi`, `sprintf` and
      unbounded `%s`:
  - `strcpy`: `9_strings.c`, `17_student_record_system.c`,
    `25_simple_text_editor.c`, `28_hash_table_implementation.c`
  - `atoi`: `15_command_line_arguments.c`, `25_simple_text_editor.c`,
    `26_simple_socket_client.c`
  - For each, either show that the destination is sized correctly (for example,
    28 mallocs `strlen + 1`), or switch to a bounded form. Lessons from 15 onward
    that parse numbers should use `strtol` with error checks. Either way, add a
    comment explaining the risk where the lesson teaches the unsafe form.
- [ ] **Chapter prose drift audit.** The prose above `## Full Source` was
      converted from lesson comments once and is not synced. Compare each chapter
      against its lesson for:
  - mis-converted structure (chapter 7 had a bullet list turned into `##`
    headings, now fixed)
  - stale claims
  - explanations that no longer match the code
  - Then consider extending `sync_chapters.py` with a heuristic warning when a
    lesson file changed more recently than its chapter's prose.

### Next (P2): tooling and quality gates

- [ ] **Stricter warnings.** `-Wshadow -Wconversion -Wsign-conversion
      -Wmissing-prototypes` currently produce 35 Clang warnings across 15 lessons.
      The worst is `28_hash_table_implementation.c` with 10. Fix them without
      making beginner lessons harder to read (an explicit cast with a comment is
      fine), then add the flags to `BASE_CFLAGS` in `scripts/smoke_check.sh`.
- [ ] **`.clang-format`** that reproduces the existing style: 4 spaces, Allman
      braces, and aligned trailing comments. Adopt it only if
      `clang-format --dry-run` produces a small diff. Then add a `make format` and
      a CI check.
- [ ] **`.clang-tidy`** with `bugprone-*`, `clang-analyzer-*` and `cert-*`,
      tuned for teaching code. Run it in CI as non-blocking at first.
- [ ] **Sanitizers for every lesson.** Compile all lessons with
      `-fsanitize=address,undefined` and run each one through a canned input path.
      Make the sanitizer block fail rather than skip on Linux CI.
- [ ] **ThreadSanitizer** run for `30_multithreaded_file_analyzer.c`.
- [ ] **Leak checking** with Valgrind or LeakSanitizer on Linux CI for the heap
      lessons: 13, 17, 20, 25, 28, 35.
- [ ] **GCC `-fanalyzer`** pass in CI, non-blocking at first.
- [ ] **`compile_commands.json`** or a `.clangd` file, so editors understand the
      lesson files out of the box.
- [ ] **CI upkeep.** Review the action versions (`actions/checkout@v4`,
      `actions/setup-node@v4`, `configure-pages@v5`, `upload-pages-artifact@v3`,
      `deploy-pages@v4`). Make
      `deploy.yml` run the sync check before publishing.
- [ ] **Book link checking.** Verify internal links and anchors. First check
      whether a link-check preprocessor supports mdBook 0.5.x; if not, write a
      small script.

### Later (P3): book and reader experience

- [ ] **Exercises.** Add a "Try it yourself" section to each chapter with 2-3
      exercises. Optionally add hidden solutions in an `exercises/` directory
      that the smoke check compiles.
- [ ] **Images.** `assets/images/` holds 7 images that the book never
      references. Use them, for example in the introduction, or delete them.
- [ ] **Per-chapter "What you'll learn / prerequisites"** box at the top of each
      chapter.
- [ ] **Glossary page** for the UPPERCASE terms the lessons introduce.
- [ ] **Setup chapter** ("Chapter 0"). The site's `/setup` page exists now (W3). Cover installing a compiler on
      macOS, Linux and Windows (WSL), using a terminal, and running a lesson. This
      depends on D1 and D3. It is now planned as the site's `/setup` page in W3;
      add it to the book too if the mdBook is kept (D6).

### Content expansion (pending D4)

Carried over from the original Phase 6 plan, plus new ideas. Each new lesson
follows the "Adding a new lesson" checklist in AGENTS.md.

- [ ] **Debugging lesson.** Cover `gdb`/`lldb` basics, breakpoints and
      backtraces, and reading sanitizer reports. It fits well in Part 3.
- [ ] **Undefined behaviour lesson.** Cover signed overflow, out-of-bounds
      access, uninitialized reads and strict aliasing, and show UBSan catching
      them.
- [ ] **Testing in C lesson.** Use plain `assert` and exit-code tests, with the
      `#define main` harness trick the smoke check already uses. Add
      `tests/test_linked_list.c` (lesson 20) and `tests/test_hash_table.c`
      (lesson 28).
- [ ] **Modern C (C23) lesson.** Cover `nullptr`, `bool`/`true` as keywords,
      `constexpr`, `typeof`, `[[nodiscard]]`, digit separators and `#embed`. This
      depends on D1.
- [ ] **Build systems beyond Make.** A lesson on CMake with Ninja, optionally
      with a root `CMakeLists.txt` and CTest registration for all lessons.
- [ ] **Networking depth.** A `poll()`-based concurrent echo server, and a Unix
      domain socket IPC example (`AF_UNIX`).
- [ ] **libsodium track** (new dependency). Cover `crypto_secretbox` symmetric
      encryption, `crypto_pwhash` (Argon2id) password hashing, and `crypto_box`
      public-key exchange, all with `sodium_memzero`. This needs a
      `pkg-config` setup, the CI package and README build notes.
- [ ] **Fuzzing.** Add libFuzzer targets for the lesson 34 data parser and the
      lesson 35 map parser in `fuzz/`, with seed corpora and a short CI run
      (`-runs=10000`).
- [ ] **Cross-compilation note.** Show building lesson 1 with `zig cc -target
      x86_64-linux-musl`.

## Ideas inbox

Unsorted ideas. Triage them into a section above, with a priority, when you
pick them up.

- **Compiler Explorer compiler pin.** "Try it online" pins GCC 16.2
  (`cg162` in `site/src/lib/compiler-explorer.ts`). Check once a year that the
  id still exists on godbolt.org and move to a newer GCC.
- **Content type for lesson downloads.** Prerendered files under
  `/lessons/<slug>/files/` are served by adapter-node without a
  `Content-Type` for `.c`, `.h` and `Makefile` (the download links still
  work through the `download` attribute). Opening one directly downloads it
  rather than showing it. Consider having Caddy serve `build/prerendered`
  for those paths with `text/plain; charset=utf-8`.
- **Chapter prose is all one style of paragraph.** The converted comments
  keep their UPPERCASE run-in headings ("WHAT IS A COMPILER?") inside
  paragraphs. On the site these could become real `###` headings, which
  would also feed the table of contents; that is a chapter prose change, so
  it belongs to the prose drift audit.

- **Linux smoke check from macOS.** Add a `make smoke-linux` target that runs
  `scripts/smoke_check.sh` in a Docker image (for example `gcc:15` plus
  `libncurses-dev` and `clang`). macOS headers define things glibc hides in
  strict `-std=` modes (lesson 32's `M_PI` passed locally but failed CI for
  several commits), so a local Linux run would catch that before pushing.

---

## Decision log

- **2026-11-20: production deploy follows the host's conventions.** The site
  runs on `ubuntu-vm` like its other sites rather than as BUILD.md first
  sketched: port 4317 (not 3000), `/srv/www/cfromthegroundup.com/releases`
  with a `current` symlink, the shared Node 24 runtime at `/opt/node-runtime`,
  credentials in `/etc/web-apps/cfromthegroundup/`, database roles
  `_owner`/`_migration`/`_web`, and `prepare.sh` + `cutover.sh` under
  `with-deploy-lock` instead of a `git pull` deploy script. D10: DNS only, so
  Caddy's automatic HTTPS.
- **2026-11-20: site scaffold choices (W1).**
  - Postgres client: **postgres.js** (`postgres`), the `sv add drizzle`
    option for PostgreSQL. It needs no native build and Drizzle supports it
    directly (`drizzle-orm/postgres-js`).
  - Node: `.nvmrc` pins **24** (production and CI); `engines` says `>=24` so
    newer local Node versions still install with `engine-strict`.
  - Local database: `site/compose.yaml` runs `postgres:18` on
    **127.0.0.1:5434**, so it does not collide with a PostgreSQL already on 5432.
  - Migrations only: the `db:push` script from the scaffold was removed so the
    schema can only change through committed `drizzle-kit generate` migrations.
  - E2E tests run against the real adapter-node server (`node build`), not
    `vite preview`.
- **2026-11-20: the website is the primary focus.** `cfromthegroundup.com`,
  self-hosted on the maintainer's Ubuntu server. The stack is SvelteKit 3,
  Svelte 5, TypeScript, Tailwind CSS 4, PostgreSQL 18 + Drizzle,
  adapter-node and Caddy. The site lives in `site/` in this repo and reads
  course content from the existing lesson and chapter files at build time.
- **2026-11-20: book listings are generated, not hand-copied.** The `.c` files
  are the single source of truth, and chapters embed them through
  `<!-- source: ... -->` markers synced by `scripts/sync_chapters.py`. CI fails on
  drift. Chosen over `{{#include}}`, which cannot handle the directory names, and
  over renaming the directories (see D2).
- **2026-11-20: lesson sources are ASCII only.** Em-dashes became `--` and `π`
  became "Pi", matching the book prose.
- **2026-11-20: mdBook is pinned to 0.5.4 in CI**, so CI matches local builds.
- **2026-05-16: license is MIT** (briefly GPL-3.0 in April 2026, then back to
  MIT).
- **2026-03-31: C23-first verification.** The smoke check prefers `-std=c23` and
  falls back to `-std=c17`, and CI runs both Clang and GCC.
- **2026-03-26: v1.0.0.** The project was converted to an mdBook published on
  GitHub Pages.

## Done log

- **2026-11-20:** milestone W4, learner experience.
  - Progress: `site/src/lib/progress/` (pure snapshot logic and a
    `localStorage` backend that syncs across tabs, behind `ProgressBackend`),
    a reactive store connected in the root layout, "Mark complete" on each
    lesson, checkmarks and per-part bars in the sidebar, progress bars on the
    overview and part pages, and "Continue where you left off" / "Up next" on
    the landing page and overview.
  - Search: `/search-index.json` (MiniSearch) over prose sections and source
    comments; a modal combobox dialog opened by the header button, `/` or
    Ctrl/Cmd+K, with arrow-key selection.
  - "Try it online": `site/src/lib/compiler-explorer.ts` builds Compiler
    Explorer client state and posts it to the shortener on click; lesson
    metadata gained `usesFiles` and `online`.
  - `j`/`k` and arrow keys move between lessons (`site/src/lib/shortcuts.ts`
    keeps them away from text fields, code blocks and tabs).
  - Zip downloads per lesson and for the whole course.
  - Tests: unit tests for progress, search, comments, zips and the Compiler
    Explorer client; e2e for each feature (Compiler Explorer is intercepted),
    and axe checks on the open search dialog and on progress UI in both
    themes. 39 e2e tests in all.

- **2026-11-20:** milestone W3, design system and core pages (Lighthouse
  measurement still open).
  - Tokens in `site/src/app.css`; components in `site/src/lib/components/`
    (header, footer, theme toggle, course sidebar, code block, copy button,
    run panel, table of contents, prev/next, badges, lesson card, page meta).
  - Routes: `/`, `/lessons`, `/lessons/[slug]`, `/lessons/[slug]/files/[name]`,
    `/parts/[n]`, `/setup`, `/about` and `+error.svelte`, all prerendered. An
    unknown lesson or part at runtime is a clean 404 (checked against a copy of
    the build outside the repository, as production runs it).
  - Shiki uses `github-dark-default`, whose comment color passes AA contrast on
    the code background.
  - e2e: landing, header navigation, theme persistence, skip link, 404, lesson
    page (listing, run panel, sidebar, prev/next), keyboard tabs, table of
    contents, downloads, mobile outline, part page, and axe-core in both
    themes (30 tests).
  - Chapters 15, 16, 25, 26, 27, 30 and 34 now show run commands with the
    arguments their programs need, and a test keeps them in step with the
    site's metadata.

- **2026-11-20:** milestone W2, course content pipeline.
  - `site/src/lib/server/content/`: `summary.ts` parses `src/SUMMARY.md`,
    `chapter.ts` renders a chapter with unified (remark-gfm, rehype-slug,
    rehype-autolink-headings) and splits it into HTML, listing and run
    segments, `highlight.ts` runs Shiki at build time (`c`, `makefile`, `sh`;
    `github-light` / `github-dark` as CSS variables), and `index.ts`
    (`getCourse()`, `getLesson()`) assembles the typed `Course`.
  - Listings are read from the lesson files themselves, not the chapter copy.
  - `metadata.ts` holds link flags, platform, interactivity and example
    arguments, and generates the compile and run commands.
  - Tests: summary parsing, chapter rendering, the whole real course (35
    lessons, every lesson file embedded once and in sync, TOC ids, prev/next),
    and agreement with `compile_lesson()` in the smoke check.

- **2026-11-20:** first production deploy (most of W5). `site/deploy/` holds
  the Caddy site, the hardened systemd unit, `provision.sh` (database, roles,
  `pg_hba`, credentials, backup timer), `prepare.sh`, `cutover.sh`,
  `rollback.sh` and the runbook. https://cfromthegroundup.com is live with a
  Let's Encrypt certificate, the `www` redirect, a healthy `/healthz` and a
  verified first backup; the other sites on the host were unaffected.
- **2026-11-20:** lesson 32 compiles on Linux again. `M_PI` is not ISO C, and
  glibc hides it under `-std=c23`, so CI's smoke check had failed on both
  compilers since at least the relicense commit, although macOS passed. The
  lesson now defines `M_PI` behind an `#ifndef` guard and explains why, and its
  wrong "defined in <math.h>" comment is fixed. Verified with GCC 15 and Clang
  on Linux.
- **2026-11-20:** milestone W1, site scaffold.
  - `site/` created with `sv create` (minimal, TypeScript) and the prettier,
    eslint, vitest, playwright, tailwindcss (typography), adapter-node and
    drizzle (PostgreSQL, postgres.js) add-ons. It uses the SvelteKit 3
    layout: config in `sveltekit({...})` in `vite.config.ts`, `#lib` subpath
    imports, and env vars in `src/env.ts`.
  - The scaffold's demo table and example tests were removed. `schema.ts` is
    empty until W6.
  - `site/compose.yaml` (PostgreSQL 18), `.env.example`, `.nvmrc`, and the npm
    scripts `dev`/`build`/`preview`/`start`/`check`/`lint`/`format`/`test`/
    `test:e2e`/`db:*`.
  - `GET /healthz` returns 200 `{"status":"ok","database":"ok"}` after a
    `select 1`, and 503 when the database is down or slower than 3 s. Unit
    tests cover the check; a Playwright test hits it on `node build`.
  - `DATABASE_URL` is optional at build time, so builds and CI need no database,
    and required and validated when the server starts.
  - Root `.gitignore`, `make site-verify` (part of `make verify`), `make
    site-e2e`, a `site` CI job on Node 24 with a `postgres:18` service, and the
    AGENTS.md "The website" section.
- **2026-11-20:** input robustness.
  - Lesson 3 now teaches that `scanf` returns the number of items read,
    covering `1`, `0` and `EOF`, and it checks every read. Variables are
    initialized.
  - Lesson 5 explains that check now that `if` has been taught.
  - Lesson 6 recovers from non-numeric input by discarding the line with
    `getchar`, and stops on EOF instead of looping forever.
  - Lesson 13 no longer passes an uninitialized count toward `malloc`.
  - Chapters 3 and 6 gained matching prose sections.
  - `run_input_checks` in the smoke check feeds those lessons valid input,
    garbage and EOF under a timeout and an output cap. Verified that it fails
    against the old lessons 3 and 6.
- **2026-11-20:** maintainability pass.
  - Added `AGENTS.md`, this roadmap, a root `Makefile` (`make verify` / `sync` /
    `serve`), `scripts/sync_chapters.py` and `.editorconfig`.
  - Added the listing check to CI and pinned mdBook.
  - Fixed lesson 1's compile command, which referenced a nonexistent
    `HelloWorld.c`.
  - Fixed chapter 7's broken list rendering.
  - Synced lesson 32's listing.
- **2026-03-31:** C23 smoke-check baseline with c17 fallback, GCC plus Clang CI
  matrix, ASan/UBSan regressions for lessons 30 and 35, and behavioural checks
  for lessons 17, 26, 29 and 30.
- **2026-03-26:** mdBook conversion with 35 chapters and a GitHub Pages deploy.
- **2025 to early 2026:** lessons 1-35 written (originally tracked here as Phases
  1-5).
