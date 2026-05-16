# C From The Ground Up

> **[Read it online](https://dunamismax.github.io/c-from-the-ground-up/)**

A progressive C learning workbook from basics to systems programming. Each lesson
is a small, readable program with the explanation in the comments and the code
inline. This is not a polished application suite. It is a progression of lessons,
exercises, and a few larger workbook-style projects.

## Structure

The book is organized into five progressive parts:

- **Part 1: The Beginner Path** -- Core language fundamentals from Hello World through strings
- **Part 2: The Intermediate Path** -- Pointers, structs, dynamic memory, file I/O, and command-line arguments
- **Part 3: The Advanced Path** -- Projects and deeper topics including function pointers, recursion, linked lists, bit manipulation, and a line-based text editor
- **Part 4: The Expert Path** -- Systems programming with sockets, process management, hash tables, and multithreading
- **Part 5: Expert Systems** -- Multi-file projects, external libraries, terminal UI with ncurses, data parsing, and a capstone text adventure

## Quick Start

Most lessons are single-file programs. Pick one `.c` file, compile it, and run it:

```sh
cc -Wall -Wextra -std=c23 \
  "Part 1 - The Beginner Path_ Core Concepts/1_hello_world.c" \
  -o /tmp/1_hello_world

/tmp/1_hello_world
```

If your compiler does not support `-std=c23` yet, use `-std=c17`.

Lesson 31 is the exception: it is intentionally split across multiple files and built with its local `Makefile`.

```sh
make -C "Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects"
```

## Build Notes

- The repo-level verification baseline prefers `-std=c23` and falls back to `-std=c17` when a compiler does not yet accept C23.
- Most lessons compile with `cc -Wall -Wextra -Wpedantic -Wstrict-prototypes -std=c23 lesson.c -o lesson_name`.
- Lessons 26 through 30 use POSIX or Unix-style APIs such as sockets, `fork`, `waitpid`, `unistd.h`, and `pthread`.
- Lesson 30 needs `-pthread`.
- Lesson 32 needs `-lm`.
- Lessons 33 and 35 need `-lncurses` or `-lncursesw`, depending on your system, so they are easiest to run on Unix-like systems or inside WSL on Windows.
- Several lessons expect runtime input or data files. Read the lesson comments before running them.

## Building the Book Locally

```sh
mdbook build
mdbook serve --open
```

## Verification

The supported verification flow is:

```sh
mdbook build
CC=clang sh scripts/smoke_check.sh
CC=gcc sh scripts/smoke_check.sh
```

If your local GCC is versioned, use that compiler name instead, for example
`CC=gcc-15 sh scripts/smoke_check.sh`.

GitHub Actions mirrors that verification path: `.github/workflows/verify.yml`
runs `mdbook build` plus the smoke check under both Clang and GCC on pushes and
pull requests.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE).
