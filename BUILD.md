# BUILD.md

The roadmap and work tracker for `c-from-the-ground-up`. Read [AGENTS.md](AGENTS.md)
first for conventions and the verification workflow.

## How to use this file

- **Picking work:** take the first unchecked item in **Now**, then **Next**. Skip
  anything that depends on an unresolved item in **Open decisions**. Those need
  the maintainer.
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
  - behavioural checks for lessons 17, 26, 29 and 30
  - ASan/UBSan regressions for lessons 30 and 35
- Code listings in the book are generated from the `.c` files by
  `scripts/sync_chapters.py`. Chapter prose is still hand-maintained and can drift.

---

## Open decisions (maintainer)

These block related backlog items. Agents: do not act on them; ask the user, or
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

## Now (P1): correctness for learners

- [ ] **Unchecked `scanf` makes interactive lessons misbehave.** In
      `6_loops.c`, a closed stdin or a non-numeric entry such as `abc` causes an
      infinite loop: the bad input stays in the buffer and the loop variable never
      changes. The same unchecked pattern is in `3_user_input.c` (lines 64, 82,
      104), `5_conditional_statements.c` (79, 106), and
      `13_dynamic_memory_allocation.c` (78). In lesson 13 a failed read leaves
      `num_students` uninitialized, and it is then used as a `malloc` size.
  - Fix: teach "`scanf` returns the number of items it read" in lesson 3,
    including the EOF case and clearing the rest of the line. Use that check
    consistently in 5, 6 and 13, and update the matching chapter prose.
  - Add a smoke check that runs each interactive lesson with `</dev/null` and
    with garbage input under a timeout, and asserts that it exits.
- [ ] **EOF and hang sweep.** Run every lesson with `</dev/null` under a 3 s
      timeout and record any that hang or crash. Lessons that need arguments
      (16, 25, 34, ...) should print usage and exit non-zero, which is fine.
      Fix the hangs.
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

## Next (P2): tooling and quality gates

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
      `configure-pages@v5`, `upload-pages-artifact@v3`, `deploy-pages@v4`). Make
      `deploy.yml` run the sync check before publishing.
- [ ] **Book link checking.** Verify internal links and anchors. First check
      whether a link-check preprocessor supports mdBook 0.5.x; if not, write a
      small script.

## Later (P3): book and reader experience

- [ ] **Exercises.** Add a "Try it yourself" section to each chapter with 2-3
      exercises. Optionally add hidden solutions in an `exercises/` directory
      that the smoke check compiles.
- [ ] **Images.** `assets/images/` holds 7 images that the book never
      references. Use them, for example in the introduction, or delete them.
- [ ] **Per-chapter "What you'll learn / prerequisites"** box at the top of each
      chapter.
- [ ] **Glossary page** for the UPPERCASE terms the lessons introduce.
- [ ] **Setup chapter** ("Chapter 0"). Cover installing a compiler on
      macOS, Linux and Windows (WSL), using a terminal, and running a lesson. This
      depends on D1 and D3.

## Content expansion (pending D4)

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

- (empty: add ideas here)

---

## Decision log

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
