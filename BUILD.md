# BUILD.md

The roadmap and work tracker for `c-from-the-ground-up`. Start with **Now**.
Read [AGENTS.md](AGENTS.md) for conventions and the verification workflow.

## Now

| | |
| --- | --- |
| **Current phase** | [Phase W: cfromthegroundup.com](#phase-w-cfromthegroundupcom-current-focus): W7 (launch) and W9 (Umami analytics), plus the [course content track](#course-content-track). D1-D10 are all settled (see the Decision log). |
| **Next step** | The **chapter prose drift audit** (read each chapter beside its lesson for tone and order), then the [Ideas inbox](#ideas-inbox), the Later items (setup chapter, prerequisites boxes, glossary, exercises), Part 6 (lessons 36-38) and W9. |
| **In progress** | Nothing claimed. When sessions run in parallel, claim an item here (its name, a one-line goal and the date) before starting, and take it off when it lands. |
| **Waiting on the maintainer** | (1) The GitHub OAuth app and `accounts.env` (W0); the steps are under "Accounts" in [`site/deploy/README.md`](site/deploy/README.md). (2) A manual screen-reader pass (W7). |
| **Live** | https://cfromthegroundup.com serves `25146a8`, deployed 2026-11-20 with `site/deploy/`, so the site matches the lessons on `main`. The database has the W6 tables; accounts are off until the OAuth app exists. |
| **`main`** | `make verify` and CI are green at `25146a8`. |
| **Last updated** | 2026-11-20 |

### Milestones at a glance

| Milestone | Status |
| --- | --- |
| [W0. Inputs from the maintainer](#w0-inputs-from-the-maintainer-agents-ask-do-not-block-on-these) | Server facts, DNS and decisions done; the OAuth app is open |
| [W1. Scaffold the app](#w1-scaffold-the-app) | Done |
| [W2. Course content pipeline](#w2-course-content-pipeline) | Done |
| [W3. Design system and core pages](#w3-design-system-and-core-pages) | Done |
| [W4. Learner experience](#w4-learner-experience-no-accounts-yet) | Done |
| [W5. Production deployment](#w5-production-deployment) | Done |
| [W6. Accounts and synced progress](#w6-accounts-and-synced-progress-built-with-d7s-default) | Done in code; off in production until the OAuth app exists |
| [W7. Launch](#w7-launch-and-make-the-site-the-front-door) | Maintainer step left: screen-reader pass |
| [W9. Analytics](#w9-analytics-d9-self-hosted-umami) | Not started |
| [W8+. Beyond parity](#w8-beyond-parity-triage-into-milestones-when-w7-is-done) | Ideas, to triage when W7 is done |

## How to work this file

1. **Orient.** Read **Now**, then run `git status --short` and
   `git log -5 --oneline`. Leave alone any changes in the tree that you did
   not make: another session may be working in this checkout.
2. **Choose.** The maintainer's request in chat comes first. Otherwise take
   **Next step**. Work Phase W's milestones in order, then the course content
   track (Now, then Next, then Later). Skip anything that waits on an open
   decision; where the decision has a stated **default**, build with the
   default and say so.
3. **Claim.** When sessions run in parallel, claim the item under
   **In progress** first, and stay out of the files another claim covers.
4. **Build and verify.** Each milestone lists what "done" means. Check it
   through the real entry point as well as the tests (see "Start and finish a
   session" in AGENTS.md), then run `make verify`.
5. **Record.** Tick the box (`- [x]`) in the same commit as the change, with a
   sentence on how it was verified. If you only partly finish, leave the box
   unchecked and add a `Remaining:` line saying exactly what is left. Add a
   short entry to the top of the **Done log**; the details belong in the
   commit body.
6. **Advance.** Update **Now**: next step, in progress, waiting on the
   maintainer, live, and the date. When a milestone's "Done when" holds, set
   its **Status** line and its row in **Milestones at a glance**.

**Changing the plan.** You may split, add or reword unchecked items when real
work calls for it; say why in the commit body. Keep items concrete: name the
files, the lessons and what "done" looks like. Never delete a checked item; if
a regression undoes one, uncheck it and say why. Deleting unfinished work,
lowering a "Done when" or changing a settled decision needs the maintainer.

**New ideas** you are not doing now go in the **Ideas inbox** with one line of
context (what, where, why), never only in chat or a commit message.

**Decisions.** When the maintainer settles an open decision, move it to the
**Decision log** with the date and outcome, then turn its consequences into
backlog items. Decisions are numbered D1-D10: D1-D5 cover course content and
D6-D10 the website.

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
- [x] Decisions D6-D10, settled 2026-11-20 (see the Decision log): keep the
      book as a mirror, GitHub sign-in only, Compiler Explorer links, Umami
      analytics, DNS only.
- [ ] A GitHub OAuth app for sign-in (W6), under the maintainer's account:
      homepage `https://cfromthegroundup.com`, callback
      `https://cfromthegroundup.com/api/auth/callback/github`. Then write
      `/etc/web-apps/cfromthegroundup/accounts.env` on the server as
      `site/deploy/README.md` ("Accounts") says. Until then accounts are off
      and the site works as in W4.

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
- [x] Performance target: Lighthouse 95+ on mobile for lesson pages.
      Measured 2026-11-20 with Lighthouse 12 (mobile, against `node build`
      locally): performance 100 on `/`, `/lessons` and lesson 1, and 99 on
      lesson 35 (the largest page); accessibility, best practices and SEO 100
      on all four. Re-measure on the production URL in the W7 launch check.
- **Done when:** every lesson is readable on the site at parity with the mdBook,
  on desktop and mobile, in both themes, and Playwright e2e tests cover the
  landing page, a lesson page and navigation.
- **Status:** done 2026-11-20.

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
- [x] Production environment template: `DATABASE_URL` in `web.env`
      (written by `provision.sh`), the optional `accounts.env` (W6), and
      `ORIGIN`, `PORT`, `HOST`, `ADDRESS_HEADER=X-Forwarded-For` and
      `XFF_DEPTH=1` in the unit (DNS only, so Caddy is the only proxy). Tested
      2026-11-20: `GET /healthz/address` reports `getClientAddress()`.
      `healthz/address/address.e2e.ts` starts `node build` with the unit's
      proxy settings and checks that the right-most `X-Forwarded-For` entry
      wins, then puts a real Caddy in front (when `caddy` is installed, as on
      the maintainer's Mac) and checks that a spoofed header is ignored.
      Caddy's documented default is to ignore incoming `X-Forwarded-*` from
      untrusted clients. `cutover.sh` repeats the spoof check through the
      production Caddy on every deploy.
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
- [x] Migrations at deploy time: `scripts/migrate.ts` (drizzle-orm's
      migrator, bookkeeping in schema `app`, `SET ROLE` to the owner) is
      bundled by `npm run build` into `build/migrate/migrate.js` with rolldown,
      so it runs without `node_modules`. `prepare.sh` checks the bundle reads
      every migration; `cutover.sh` takes a backup and runs it before the
      switch, as `cfromthegroundup_migration` in a sandboxed `systemd-run` unit
      reading `migration.env`. Rehearsed locally against roles set up exactly
      as `provision.sh` does: the tables belong to the owner, the web role can
      read and write rows but not create tables, and a second run applies
      nothing. The runbook explains forward-only, backward-compatible
      migrations.
- [x] PostgreSQL 18 setup (`site/deploy/provision.sh`; 18 was already
      installed from Ubuntu's archive):
  - install from Ubuntu's archive if it ships 18, otherwise from the PGDG apt
    repository
  - a dedicated database and role, local-only connections
  - a nightly `pg_dump` systemd timer with rotation
    (`cfromthegroundup-backup.timer`, 30 days in `/var/backups/cfromthegroundup`)
- [x] Use CSP in SvelteKit's `csp` config (nonce- or hash-based) rather than a
      static Caddy header, so inline scripts SvelteKit generates keep working.
      Done: `mode: 'hash'` in `vite.config.ts` (a `<meta>` tag on prerendered
      pages, a header on dynamic ones). The hash of the theme script in
      `app.html` is computed from the file at build time. `style-src` allows
      inline styles (Shiki token colors); `connect-src` allows
      `https://godbolt.org` for "Try it online". `csp.e2e.ts` fails on any
      violation.
- **Done when:** `https://cfromthegroundup.com` serves the site with valid TLS,
  `www` redirects, `/healthz` is green, a redeploy is one command, and a
  database backup exists.
- **Status (2026-11-20):** done. TLS, the `www` redirect, `/healthz` and a
  backup are verified live. A redeploy is two commands (prepare, then
  cutover). The `de586a7` deploy ran the migrator for the first time (backup
  first, then 1 migration applied, tables owned by `cfromthegroundup_owner`)
  and the client-IP check through the production Caddy passed.

#### W6. Accounts and synced progress (built with D7's default)

- [x] Authentication with Better Auth 1.7 and its Drizzle adapter, GitHub
      sign-in only (D7's default). Installed by hand rather than with
      `sv add better-auth`, which adds demo routes. Config in
      `src/lib/server/auth-options.ts` (sessions 30 days, GitHub tokens stored
      encrypted, rate limits, telemetry off), the instance in `auth.ts`,
      mounted under `/api/auth/` in `hooks.server.ts`. Accounts are on only
      when all four `BETTER_AUTH_*` / `GITHUB_*` variables are set.
- [x] A `lesson_progress` table (user, lesson slug, `completed_at`,
      `last_viewed_at`, primary key on user and slug), with Better Auth's
      tables, all in schema `app` (migration `0000_accounts_and_progress`).
      The progress store talks to it through `GET`/`POST`/`PUT /api/progress`
      (see the Decision log for why not remote functions), via
      `src/lib/progress/account-backend.ts`.
- [x] Merging: progress in `localStorage` is merged into the account on first
      sign-in (union of completions, newest view), and again whenever this
      browser changed completions while signed out or offline. Otherwise the
      account wins, so "not complete" on one device sticks on the others.
- [x] Rate limits: Better Auth's own (10 a minute for sign-in and callbacks,
      60 for the rest of `/api/auth/`) plus 120 writes a minute per client
      address on all of `/api/` (`src/lib/server/rate-limit.ts`).
- [x] Account page (`/account`): sign in, progress, download your data (JSON,
      no secrets), sign out, delete the account (cascades to sessions, the
      GitHub link and progress).
- [x] Privacy policy (`/privacy`): what is stored with and without an account,
      where, for how long, and how to export or delete it. Linked from the
      footer, the account page and About.
- **Done when:** progress follows a signed-in learner across devices, and
  signed-out learning still works exactly as in W4.
- **Status (2026-11-20):** done in code. `account/account.e2e.ts` signs
  browsers in with Better Auth's test helpers and covers the GitHub redirect,
  the first-sign-in merge, two browsers syncing both ways, sign-out,
  export and deletion, API validation and axe on the signed-in page; the
  W4 tests still pass with accounts on. Production needs the GitHub OAuth app
  (W0) before anyone can sign in.

#### W7. Launch and make the site the front door

- [x] SEO basics: per-page `<title>`, description, canonical URL and Open
      Graph tags (`PageMeta.svelte`); `sitemap.xml` (prerendered) and
      `robots.txt` pointing at it; `Course` JSON-LD on the landing page and
      `LearningResource` plus `BreadcrumbList` on lesson pages
      (`site/src/lib/structured-data.ts`).
- [x] Open Graph images, generated at build time: `/og/site.png` and one per
      lesson (`/og/<slug>.png`, 36 in all, 1.5 MB), prerendered by
      `src/routes/og/[name].png/` with satori and `@resvg/resvg-wasm`
      (`src/lib/server/og.ts`). Fonts come from the `@fontsource` packages
      (Inter, JetBrains Mono; OFL), so no font file is committed. `PageMeta`
      sets `og:image` and `twitter:card`.
- [x] Point everything at the site:
  - [x] set the repo homepage to `https://cfromthegroundup.com`. Done
        2026-11-20 with the maintainer's go-ahead (`gh repo edit
        --homepage`); `gh repo view --json homepageUrl` returns it.
  - [x] the site is the first link in `README.md`
  - [x] a banner in `src/index.md`, and D6's default applied to the mdBook:
        every book page has a link to the same lesson on the site
        (`book-theme/site-link.js` and `.css`, through `additional-js` in
        `book.toml`)
- [x] Map old mdBook URLs (`/c-from-the-ground-up/chapters/NN-slug.html`) to
      the new lesson URLs: with D6's default (keep the book), each chapter
      page links to `/lessons/NN-slug`. Chapter file names are the lesson
      slugs, which the site's content tests enforce. If D6 becomes "redirect",
      the same mapping can redirect instead.
- [x] Final launch check: accessibility, performance and broken-link passes on
      the production URL. Done 2026-11-20 against commit `a14e994`:
  - links: every page in `sitemap.xml` (45) returns 200, and so do all 288
    distinct same-origin links and assets on them, with every `#anchor`
    present; all 45 distinct external URLs return 200
  - accessibility: axe-core (WCAG 2.2 AA tags) on all 45 pages plus a 404, in
    both themes: no violations and no console errors
  - performance: Lighthouse 12.8 mobile on `/`, `/lessons`, lesson 1 and
    lesson 35 scores 100 for performance, accessibility, best practices and
    SEO (a cold first run of `/` scored 79 on total blocking time; three
    re-runs scored 100)
- [ ] Manual screen-reader pass (VoiceOver or NVDA) over the landing page, a
      lesson page, search and the run panel. Needs a person; automated checks
      cannot judge it.
- **Done when:** the site is the documented primary way to take the course.

#### W9. Analytics (D9: self-hosted Umami)

- [ ] Umami runs on `ubuntu-vm` as its own systemd service on a local port,
      with its own database and role in the shared PostgreSQL 18, set up by a
      script in `site/deploy/` (no other site's files touched).
- [ ] The tracker reaches Umami through the site's own origin (a Caddy
      `handle` for the script and the collect endpoint), so no new DNS record
      and no new CSP origin are needed. The dashboard is reached over an SSH
      tunnel, not published.
- [ ] The site loads the tracker only when `PUBLIC_UMAMI_WEBSITE_ID` is set,
      respects Do Not Track, sets no cookies, and the CSP allows it; e2e
      tests cover both on and off.
- [ ] `/privacy` says what Umami records (page, referrer, browser, country;
      no cookies, no IP stored) and the runbook says how to install, back up
      and update it.
- **Done when:** page views from the live site show up in Umami, and the site
  works exactly as before when Umami is down or switched off.

#### W8+. Beyond parity (triage into milestones when W7 is done)

- Exercises per lesson with automated checking (see "Exercises" in Later), and
  short end-of-lesson quizzes.
- In-browser compile and run: not planned. D8 keeps the Compiler Explorer
  link-out.
- Completion certificates per part and for the whole course.
- Notes and bookmarks per lesson (needs W6).
- Self-hosted, privacy-friendly analytics: chosen by D9, now milestone W9.
- Offline support (PWA), and a printable or PDF edition generated from the same
  content.
- Translations, if the audience asks for them.

### Website decisions (maintainer)

All settled on 2026-11-20; the outcomes are in the Decision log.

- [x] **D6. Future of the mdBook on GitHub Pages:** keep it as a mirror, with
      the banner on every page linking to the site.
- [x] **D7. Sign-in methods:** GitHub OAuth only, as built in W6.
- [x] **D8. Running C in the browser:** keep linking out to Compiler Explorer
      (W4). No backend compile service.
- [x] **D9. Analytics:** self-hosted Umami, cookieless (see W9).
- [x] **D10. Cloudflare proxy and TLS:** DNS only, so Caddy's automatic HTTPS
      is used.

---

## Course content decisions (maintainer)

All settled on 2026-11-20; the outcomes are in the Decision log.

- [x] **D1. C standard:** the lessons teach `-std=c17`. The smoke check
      compiles every lesson with it and again with `-std=c23`.
- [x] **D2. Directory names:** keep the `Part N - ...` names, so external
      links keep working. The sync script stays.
- [x] **D3. Windows:** WSL is the official Windows path for the whole course.
- [x] **D4. New content:** the debugging, undefined behaviour and testing
      lessons, as a new Part 6 (lessons 36-38). The other tracks stay in
      **Content expansion** as ideas, not planned work.
- [x] **D5. `@date` headers:** keep them as the date a lesson was first
      published; do not bump them on edits.

---

## Course content track

Secondary to Phase W. These items still matter: the website is only as good as
the lessons it serves.

### Now (P1): correctness for learners

- [x] **Unchecked `scanf` made interactive lessons misbehave.** Done
      2026-11-20; see the Done log.
- [x] **EOF and hang sweep for the remaining lessons.** Done 2026-11-20: every
      lesson except 26 (needs a server) and 33 and 35 (ncurses, need a
      terminal) was run with empty stdin and with garbage input from a file
      under a 5-second limit, and lesson 25 also with a file argument and
      valid commands. All exit cleanly except lesson 29, which looped forever
      when its commands came from a file: after a failed `execvp` the child
      called `exit()`, whose stdio cleanup rewound the input it shares with
      the parent. It now calls `_exit(127)` and explains why, chapter 29 says
      so, and the smoke check feeds it unknown commands from a file (that
      check fails against the old lesson). Lessons that need arguments print
      usage and exit non-zero. 33 and 35 have run in a pseudo-terminal in the
      sanitizer check since 2026-11-20.
- [x] **Uninitialized-read detection.** Done 2026-11-20: MemorySanitizer runs
      in `scripts/sanitizer_check.sh` (see the Done log).
- [x] **Unsafe-API audit.** Done 2026-11-20; see the Done log.
- [ ] **Chapter prose drift audit.** The prose above `## Full Source` was
      converted from lesson comments once and is not synced. Compare each chapter
      against its lesson for:
  - mis-converted structure (chapter 7 had a bullet list turned into `##`
    headings, now fixed; on 2026-11-20 chapters 5, 6 and 19 got code fences
    for their syntax examples, 17, 20 and 35 lost stray half-sentences, and
    34's banner line became a heading. A scan for unfenced code and fragment
    paragraphs is clean now, but no chapter has been read side by side with
    its lesson yet. The scan first missed chapter 6's opening `printf`
    example, which is fenced now too; a second scan for bare statement lines
    found nothing else.)
  - stale claims
  - explanations that no longer match the code
  - 2026-11-20, automated cross-check of all 35 chapters: every inline code
    span and fenced snippet in the prose appears in the chapter's lesson
    sources (one generic `sizeof(buffer) - 1` in chapter 26 now names the
    real buffers); the 64 prose sentences not found in the lesson comments
    are text written during later fixes, and the factual ones were checked
    against the code; every "Lesson N" cross-reference in lessons and
    chapters names a lesson that teaches what it says (lesson 13's new
    `size_t` note was corrected). Still open: a human read of each chapter
    next to its lesson, for tone and order rather than facts.
  - Then consider extending `sync_chapters.py` with a heuristic warning when a
    lesson file changed more recently than its chapter's prose.

### Next (P2): tooling and quality gates

- [x] **Stricter warnings.** Done 2026-11-20; see the Done log.
- [x] **`.clang-format`** that reproduces the existing style. Done
      2026-11-20 (see the Done log).
- [x] **`.clang-tidy`** with `bugprone-*`, `clang-analyzer-*` and `cert-*`,
      tuned for teaching code. Done 2026-11-20 (see the Done log). It is
      blocking from the start, since it is clean and pinned.
- [x] **Sanitizers for every lesson.** Done 2026-11-20 (see the Done log).
      Lessons 33 and 35 run in a pseudo-terminal (`scripts/pty_run.py`);
      MemorySanitizer skips them, since ncurses is not instrumented.
- [x] **ThreadSanitizer** run for `30_multithreaded_file_analyzer.c`. Done
      2026-11-20.
- [x] **Leak checking** with LeakSanitizer for every lesson it can run,
      including the heap lessons 13, 17, 20, 25, 28 and 35's loader. Done
      2026-11-20.
- [x] **GCC `-fanalyzer`** pass in CI. Blocking since 2026-11-20: the step
      passed on every CI run since it was added, and GCC 14 (on CI), 15 and
      16 report nothing, so `continue-on-error` is gone.
- [x] **`compile_commands.json`** or a `.clangd` file. Done 2026-11-20: a
      root `.clangd` adds the smoke check's flags (without `-Werror`).
- [x] **CI upkeep.** Done 2026-11-20; see the Done log. Review the action
      versions again once a year.
- [x] **Book link checking.** Done 2026-11-20; see the Done log.

### Later (P3): book and reader experience

- [ ] **Exercises.** Add a "Try it yourself" section to each chapter with 2-3
      exercises. Optionally add hidden solutions in an `exercises/` directory
      that the smoke check compiles.
- [x] **Images.** Done 2026-11-20: the 7 unused images in `assets/images/`
      were deleted at the maintainer's request (unused, and several were
      third-party artwork with no clear license). `assets/images/site/` keeps
      the README screenshots.
- [ ] **Per-chapter "What you'll learn / prerequisites"** box at the top of each
      chapter.
- [ ] **Glossary page** for the UPPERCASE terms the lessons introduce.
- [ ] **Setup chapter** ("Chapter 0") for the book. The site's `/setup` page
      (W3) already covers this. The mdBook is kept (D6), so add a matching
      chapter: installing a compiler on macOS, Linux and Windows (WSL), using a
      terminal, and running a lesson, teaching `-std=c17` (D1) and WSL on
      Windows (D3).

### Content expansion

D4 (2026-11-20) picked the first three items below, as Part 6, lessons 36-38.
The rest are ideas, not planned work; each needs the maintainer's go-ahead
before anyone starts it. Each new lesson follows the "Adding a new lesson"
checklist in AGENTS.md.

- [ ] **Debugging lesson** (Part 6, lesson 36). Cover `gdb`/`lldb` basics, breakpoints and
      backtraces, and reading sanitizer reports.
- [ ] **Undefined behaviour lesson** (Part 6, lesson 37). Cover signed overflow, out-of-bounds
      access, uninitialized reads and strict aliasing, and show UBSan catching
      them.
- [ ] **Testing in C lesson** (Part 6, lesson 38). Use plain `assert` and exit-code tests, with the
      `#define main` harness trick the smoke check already uses. Add
      `tests/test_linked_list.c` (lesson 20) and `tests/test_hash_table.c`
      (lesson 28).
- [ ] **Modern C (C23) lesson.** Cover `nullptr`, `bool`/`true` as keywords,
      `constexpr`, `typeof`, `[[nodiscard]]`, digit separators and `#embed`. This
      D1 teaches C17, so this lesson would give the `-std=c23` command.
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
- **Lesson 35's input window.** It is 2 rows high and `box()` draws both its
  borders, so the prompt is printed on the bottom border. A 3-row window (or
  no box) would give the input a real line; it needs a look in a terminal.
- **Lesson 25 splits long lines on load.** `load_file` reads with a 1024-byte
  `fgets` buffer, so a longer line becomes several lines and saving changes
  the file. The lesson now says so; a `realloc`-based reader would fix it
  but adds code to a Part 3 capstone.

---

## Decision log

- **2026-11-20: the maintainer settled D1-D9.**
  - D1: the lessons teach **C17**. Every `-std=c11` became `-std=c17`, lesson 1
    explains the standards, and the site's `C_STANDARD` is `c17`. The smoke
    check now compiles with `-std=c17` and, when the compiler accepts it,
    again with `-std=c23`; the sanitizer check, clang-tidy and the analyzer
    use C17.
  - D2: keep the directory names. External links matter more than shorter
    paths; the sync script stays.
  - D3: **WSL** is the official Windows path. Run lines say "On Windows
    (WSL)", not `.exe`.
  - D4: build the debugging, undefined behaviour and testing lessons as a new
    **Part 6** (lessons 36-38), appended so no published URL moves. The other
    content tracks stay as ideas.
  - D5: keep `@date` as the first-published date.
  - D6: keep the mdBook as a mirror with the banner. So the book gets a setup
    chapter too.
  - D7: GitHub sign-in only.
  - D8: keep the Compiler Explorer link-out.
  - D9: self-hosted **Umami** analytics (W9). The images in `assets/images/`
    were deleted at the same time.
- **2026-11-20: W6 uses `+server.ts` JSON endpoints, not remote functions.**
  BUILD.md planned remote functions for progress, but in SvelteKit 3.1 they
  are still experimental and need Svelte's experimental async mode across the
  app. Three small endpoints under `/api/progress` do the same job with stable
  APIs; JSON bodies cannot be sent cross-site without CORS, so they are safe
  from CSRF. Revisit when remote functions are stable.
- **2026-11-20: W6 built with D7's default (GitHub sign-in only)**, so the
  code is ready; the maintainer can still pick other methods. Accounts are off
  until the GitHub OAuth app and `accounts.env` exist.
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

Newest first. One short entry per piece of work: what changed, why, and how it
was checked. The full detail belongs in the commit body.

- **2026-11-20:** the maintainer settled D1-D9 and set the repo homepage.
  D1: 123 `-std=c11` commands became `-std=c17` (lessons, chapters, lesson
  31's Makefile, the site's `C_STANDARD` and its tests), lesson 1 explains the
  C standards, and the README and book intro teach C17. The smoke check now
  requires `-std=c17` and compiles all 37 source files again with
  `-std=c23`; the sanitizer check, clang-tidy and the analyzer use C17. D3:
  the 28 "On Windows: `x.exe`" lines say "On Windows (WSL): `./x`". D5 is
  written into AGENTS.md. The 7 unused images were deleted, and the GCC
  analyzer step is blocking (it passed on every CI run). Checked with `make
  verify`.

- **2026-11-20:** sixth production deploy, commit `25146a8`, which brings the
  site up to date with the lessons (stricter-warning edits, lesson 21's
  integer promotion, lesson 35's checked `fseek`, chapter prose fixes).
  Prepare and cutover passed first time (backup taken, no new migrations).
  Checked from outside: `/version.txt`, `/healthz`, `/api/account` (off), a
  spoofed `X-Forwarded-For` ignored, the `www` redirect, a runtime 404, the
  new text on lessons 18, 21 and 35 and in a file download, the search index,
  sitemap, course zip and an Open Graph image. The launch-check link crawl
  (45 pages, 289 internal and 45 external URLs) and axe in both themes (92
  runs) were clean again.

- **2026-11-20:** static analysis. `.clang-tidy` turns on `bugprone-*`,
  `clang-analyzer-*` and `cert-*`; of 147 findings, all but one came from
  seven checks that flag what the course teaches on purpose or a safe idiom
  (unchecked `printf` returns, `strcpy`/`memcpy`, `1 << 7` masks, `scanf`
  conversions, `void *` conversions, `buf[strcspn(...)]`), which are off with
  the reason written next to each. The one real finding: lesson 35 used
  `rewind`, which cannot report failure; it now uses a checked
  `fseek(file, 0, SEEK_SET)` as lesson 30 does. `make tidy` (in `make
  verify`) and the new `analyze` CI job run clang-tidy 23 (pinned from PyPI)
  and fail on any finding; `make analyze` runs GCC's `-fanalyzer`, clean
  with GCC 15 and 16, and non-blocking in CI for now. Both were checked to
  fail on a planted null dereference.

- **2026-11-20:** `.clang-format` and `.clangd`. The format (LLVM base,
  4-space indents, Allman braces, right-hand pointers, no column limit, no
  comment reflow, includes left in order) differed from the lessons in only
  12 lines, all trailing-comment alignment in lessons 25, 30 and 35, which
  were reformatted. `make format` applies it, `make check-format` is part of
  `make verify`, and the mdbook CI job checks it with clang-format 23.1.5
  from PyPI. `.clangd` gives editors the smoke check's warning flags.

- **2026-11-20:** sanitizer coverage for the lessons.
  `scripts/sanitizer_check.sh` (`make sanitize`, part of `make verify`, and a
  `sanitizers` CI job with `SANITIZERS_REQUIRED=1`) builds every lesson but
  33 with ASan, UBSan and LeakSanitizer and runs each with canned valid and
  bad input (lesson 26 as a server and client pair; 33 and 35 in an 80x24
  pseudo-terminal through `scripts/pty_run.py`, which types their keys),
  repeats the non-terminal runs under MemorySanitizer, and runs lesson 30
  under ThreadSanitizer. A harness also loads lesson 35's world from a good
  map, two bad ones and a missing file and frees it. Every lesson is clean
  (90 runs on Linux with Clang 19; macOS skips MSan). The terminal run was
  checked to catch an injected heap overflow in lesson 35's `look`. Checked that it fails on
  an injected leak (LeakSanitizer names `create_node` in lesson 20) and on an
  injected uninitialized read (MemorySanitizer names the line and the
  allocation). `make sanitize-linux` runs it in Docker; MSan needs
  `--security-opt seccomp=unconfined` there, and the CI job lowers
  `vm.mmap_rnd_bits` so MSan and TSan can map their shadow memory.

- **2026-11-20:** stricter warnings, CI upkeep, book link checking, a Linux
  smoke target, and the automated part of the W7 launch check.
  - Stricter warnings: `-Wshadow -Wconversion -Wsign-conversion
    -Wmissing-prototypes` are in `BASE_CFLAGS`. The 33 Clang warnings (plus
    GCC's `htons` ones) are fixed: lessons 16, 18-21, 23-25 and 28-30 gained
    a `// --- Function Prototypes (Lesson 7) ---` block, and each narrowing
    conversion is a cast with a comment saying why the value fits (13, 17,
    21, 26, 28, 35). Lesson 21 now teaches INTEGER PROMOTION where `~a` and
    `c << 2` need a cast back to `uint8_t`, and chapter 21 has a matching
    section (its operator list, which rendered as one paragraph, is a real
    list now). The lesson 35 harness declares its renamed `main`. Checked
    with Clang and GCC 16 on macOS and GCC 15 and Clang on Linux.
  - `make smoke-linux` runs the smoke check under glibc in Docker (`gcc:15`
    with Clang added), from the Ideas inbox.
  - CI: `actions/checkout@v7`, `setup-node@v7`, `configure-pages@v6`,
    `upload-pages-artifact@v5`, `deploy-pages@v5` (all Node 24; the only
    breaking change that touches us is upload-pages-artifact dropping
    dotfiles since v4, and the book's one dotfile, `.nojekyll`, is not
    needed for an Actions deploy). `deploy.yml` checks the listings before
    publishing and can be run by hand (`workflow_dispatch`).
  - `scripts/check_book_links.py` checks every internal link, asset and
    `#anchor` in the built book (1049 links in 37 pages, all fine), runs in
    `make book` and CI, and was checked to fail on a missing file and on
    missing anchors. No mdBook 0.5 link-check preprocessor is maintained.
  - W7 launch check on production: links, axe and Lighthouse all clean (see
    W7). Only a manual screen-reader pass is left.
  - The Caddy e2e test (`healthz/address/address.e2e.ts`) could fail with
    ENOTEMPTY when it deleted Caddy's data directory while Caddy was still
    exiting; it now waits for the exit (20 repeated runs pass).

- **2026-11-20:** fifth production deploy, commit `a14e994` (chapter 6's
  opening `printf` example in a code fence). Prepare and cutover passed on
  the first try with the fixed checks (backup taken, no new migrations).
  Checked from outside: `/version.txt`, `/healthz`, `/api/account` (off),
  the spoofed `X-Forwarded-For`, the `www` redirect, every page type, the
  search index, sitemap, zip, an Open Graph image, a runtime 404, chapter 6
  rendering its example as a code block, and no console errors. The book's
  Pages deploy for `a14e994` failed in GitHub (see CI upkeep); this record's
  push publishes it.

- **2026-11-20:** fourth production deploy, commit `9412c1b` (the lesson
  correctness fixes from `46208f2`). The first attempt, at `46208f2`, failed
  cutover's home-page check and rolled itself back to `de586a7` with the site
  up throughout: the check piped curl into `grep -q` under `pipefail`, so
  when grep exited at the match curl failed writing (error 23) and
  `local_get` retried until it gave up. `9412c1b` fetches pages in full before
  matching (`cutover.sh` and `prepare.sh`). Its prepare and cutover passed,
  including all of cutover's checks (no new migrations; backup taken).
  Checked from outside: `/version.txt`, `/healthz`, `/api/account` (off), the
  client address with a spoofed `X-Forwarded-For`, the `www` redirect, every
  page type, the search index, sitemap, zip and file downloads, an Open Graph
  image, a runtime 404, the new lesson text, and no console errors on a
  lesson page.

- **2026-11-20:** lesson correctness review of all 35 lessons, and the
  unsafe-API audit.
  - Bugs fixed: lesson 25 parsed bytes past the end of the text for an empty
    line or a bare command, ran the rest of an overlong line as a command, and
    accepted "pq" and "d abc"; lesson 26 printed unterminated buffers when a
    full 2000 bytes arrived, never validated the client's IP or port, and left
    `sockaddr_in` padding uninitialized; lesson 28 did not check `malloc`;
    lesson 30 joined threads that `pthread_create` failed to start; lesson 33
    kept drawing after `endwin()` when colors were missing and used `attron`
    on the wrong window; lesson 35 acted on an unfilled buffer when
    `wgetnstr` failed, drew wrapped log messages over each other, and did not
    check `newwin`. Lessons 14, 17 and 25 now check `fclose` after writing.
  - Wrong comments fixed in lessons 4 (`a / 3`; "no boolean type"), 11
    (`scanf` with a string in Lesson 3), 13 (the `malloc` cast is required),
    16 (`10 plus 5` gives the usage message), 20, 22 and 31 (`"..."` includes
    search the current directory), 23, 26 ("this lesson and the next"), 28
    (the hash "sums" characters; which keys collide), 30, 32 (`-lm` on
    macOS), 33 (`exit` not taught yet) and 35 ("sscanf is used").
  - Unsafe APIs: `atoi`/`atof` replaced by `strtol`/`strtod` with checks in
    15, 16, 25 and the 26 client (lesson 15 now teaches why); every remaining
    `strcpy`/`strcat` (9, 12, 17, 25, 28, 35) has a comment showing the
    destination is big enough; 34's `strtok` example is bounded. No lesson uses
    `sprintf`, `gets` or an unbounded `%s` read.
  - New smoke checks for lessons 15, 16, 25, 26 (client arguments) and 28;
    each was verified to fail against the old lesson. Smoke check run on macOS
    (Clang, GCC 16) and on Linux in `gcc:15` (GCC 15, Clang).

- **2026-11-20:** third production deploy, commit `de586a7` (W5's migrator
  and client-IP check, W6, W7's Open Graph images). `prepare.sh` passed,
  including the migrator's `--check`; cutover took a backup, applied
  `0000_accounts_and_progress`, and its checks passed (`/version.txt`,
  `/healthz`, `/api/account`, `/healthz/address` with a spoofed
  `X-Forwarded-For`, the `www` redirect). Checked from outside: a spoofed
  `X-Forwarded-For` is ignored and the app sees the real client address;
  `/account` and `/privacy` render; `/api/account` says accounts are off;
  `/api/progress` is 401 without a session; the Open Graph images are served;
  no console errors on a lesson or the account page. The book on GitHub Pages
  shows the link to the site.

- **2026-11-20:** most of W7: Open Graph images for every page and lesson,
  the site first in `README.md`, a banner in `src/index.md`, and a link on
  every book page to the same lesson on the site (D6's default).

- **2026-11-20:** W5 finished in code and W6 built.
  - W5: `/healthz/address` and its tests (adapter-node behind the unit's proxy
    settings, and behind a real Caddy); the bundled migrator, run by
    `cutover.sh` after a backup; `prepare.sh` checks it.
  - W6: Better Auth with GitHub, schema `app` with Better Auth's tables and
    `lesson_progress`, `/api/account` (state, export, delete),
    `/api/progress`, synced progress with merge on sign-in, rate limits,
    `/account` and `/privacy`, an optional `accounts.env` in the unit, and the
    runbook's "Migrations" and "Accounts" sections. 19 new unit tests (sync,
    merging, rate limits, config) and 8 account e2e tests; the whole suite
    (63 e2e tests) is green with accounts on.

- **2026-11-20:** second production deploy, commit `ae2d9cf` (the course site:
  W2-W4, CSP, sitemap and structured data, lesson 29 fix). Prepare and cutover
  ran cleanly. Checked live: `/version.txt`, `/healthz` (database ok), every
  page type, the search index, sitemap, robots.txt, zip and file downloads,
  Brotli on pages, the CSP `<meta>`, runtime 404s, the `www` redirect, and no
  console errors on a lesson page.

- **2026-11-20:** EOF and hang sweep of the lessons (see Now). Fixed lesson
  29's infinite loop on file input (`_exit` after a failed `execvp`) and
  added a smoke-check regression for it.

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
