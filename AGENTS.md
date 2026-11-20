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
.github/workflows/verify.yml                        CI: listing check, mdbook build, smoke check (clang + gcc)
.github/workflows/deploy.yml                        publishes the book to GitHub Pages on push to main
assets/images/                                      images (not yet used in the book)
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
make verify              # check-sync + mdbook build + smoke check under clang and $(GCC)
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
  If you change those messages, update the script in the same commit.
- Runs ASan/UBSan regressions when the compiler supports them: lesson 30 on an
  empty file, and a harness that includes lesson 35 with `#define main ...` and
  calls `parse_room()` / `parse_link()` directly. Renaming or changing the
  signatures of those functions, or of `GameState` / `Room`, breaks the harness.

Requirements: a C compiler (Clang and GCC in CI), ncurses headers
(`libncurses-dev` on Debian/Ubuntu), `make`, Python 3.8+, and mdBook **0.5.4**.
CI pins mdBook in both workflows. Bump the pin deliberately, in both files, after
`make verify` passes locally with the new version.

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
- Interactive lessons must not hang when stdin hits EOF. Check `scanf` and
  `fgets` return values.
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
