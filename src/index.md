# C From The Ground Up

> **The course has a new home: [cfromthegroundup.com](https://cfromthegroundup.com).**
> It has the same lessons, plus progress tracking, search, and a "Try it online"
> button for most lessons. This book edition stays here as a mirror.

A progressive C learning workbook from basics to systems programming.

Each lesson is a small, readable program with the explanation in the comments
and the code inline. The workbook is meant to stay close to the code that is
actually here. This is not a polished application suite or portfolio showcase.
It is a progression of lessons, exercises, and a few larger workbook-style projects.

## How This Book Works

Every chapter follows the same pattern:

1. A prose explanation of the concepts being taught
2. The full, runnable C source file with detailed comments
3. Compile-and-run instructions

The original `.c` files in the repository are the single source of truth. The
comments *are* the lesson content. Read the chapter prose for the overview, then
study the source to see it in action.

## Structure

The book is organized into six progressive parts:

- **Part 1: The Beginner Path** -- Core language fundamentals from Hello World
  through strings
- **Part 2: The Intermediate Path** -- Pointers, structs, dynamic memory, file
  I/O, and command-line arguments
- **Part 3: The Advanced Path** -- Projects and deeper topics including
  function pointers, recursion, linked lists, bit manipulation, and a
  line-based text editor
- **Part 4: The Expert Path** -- Systems programming with sockets, process
  management, hash tables, and multithreading
- **Part 5: Expert Systems** -- Multi-file projects, external libraries,
  terminal UI with ncurses, data parsing, and a capstone text adventure
- **Part 6: The Toolsmith Path** -- Debugging with gdb and lldb, undefined
  behavior and the sanitizers, and testing in C

## Quick Start

Most lessons are single-file programs. Pick one `.c` file, compile it, and run it:

```sh
cc -Wall -Wextra -std=c17 \
  "Part 1 - The Beginner Path_ Core Concepts/1_hello_world.c" \
  -o /tmp/1_hello_world

/tmp/1_hello_world
```

On Windows, take the course inside WSL (the Windows Subsystem for Linux), the
supported way to run every lesson there. The site's
[setup guide](https://cfromthegroundup.com/setup) shows how to install a
compiler on macOS, Linux and Windows.

## Build Notes

- The lessons teach C17 (`-std=c17`), which every current compiler accepts. CI compiles every lesson with `-std=c17` and again with `-std=c23`, so the course also builds on the newest standard.
- Most lessons compile with `cc -Wall -Wextra -Wpedantic -Wstrict-prototypes -std=c17 lesson.c -o lesson_name`.
- Lessons 26 through 30 use POSIX APIs (sockets, `fork`, `waitpid`, `pthread`).
- Lesson 30 needs `-pthread`.
- Lesson 32 needs `-lm`.
- Lessons 33 and 35 need `-lncurses` or `-lncursesw`, depending on your system.

## Verification

The supported verification flow is:

```sh
make verify
```

That checks that every listing in this book matches its `.c` file, builds the
book, and runs the smoke check under both Clang and GCC. GitHub Actions runs
the same checks on every push and pull request.

## License

This project is licensed under the MIT License.
