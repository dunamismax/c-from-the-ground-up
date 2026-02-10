# C From The Ground Up

Project-based C curriculum for learning programming fundamentals through runnable single-file lessons and progressively advanced systems exercises.

![C11](https://img.shields.io/badge/C-C11-blue)
![Compiler](https://img.shields.io/badge/Compiler-GCC%20or%20Clang-informational)
![License](https://img.shields.io/badge/License-MIT-green)

## Quick Start

### Prerequisites

- GCC 11+ or Clang 15+
- GNU Make (needed for the multi-file Makefile lesson)

### Run Your First Lesson

```bash
git clone https://github.com/dunamismax/c-from-the-ground-up.git
cd c-from-the-ground-up
cd 'Part 1 - The Beginner Path_ Core Concepts'
gcc -Wall -Wextra -std=c11 -o lesson1 1_hello_world.c
./lesson1
```

Windows PowerShell run command:

```powershell
.\lesson1.exe
```

Expected result: a successful compile and a hello-world style console output.

## Features

- 35 progressive lessons across beginner, intermediate, advanced, and systems topics.
- Single-file teaching format so each lesson can be compiled and explored in isolation.
- Practical projects including a socket client/server, tiny shell, and text editor.
- Dedicated multi-file build lesson with a working `Makefile` example.
- Emphasis on memory handling, pointers, file I/O, and low-level programming habits.

## Tech Stack

| Layer | Technology | Purpose |
|---|---|---|
| Language | [C11](https://en.wikipedia.org/wiki/C11_(C_standard_revision)) | Core implementation language |
| Compiler | [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/) | Build and run lessons |
| Build Tool | [GNU Make](https://www.gnu.org/software/make/) | Multi-file build workflow |
| Terminal UI | [ncurses](https://invisible-island.net/ncurses/) | Advanced terminal UI lesson |
| Systems APIs | POSIX sockets and threads | Networking and concurrency lessons |

## Project Structure

```text
c-from-the-ground-up/
├── Part 1 - The Beginner Path_ Core Concepts/                 # Intro lessons 1-9
├── Part 2 - The Intermediate Path_ Deeper into C/             # Pointers, structs, dynamic memory
├── Part 3 - The Advanced Path_ Towards Mastery/               # Projects and advanced language features
├── Part 4 - The Expert Path_ Systems and Concurrency/         # Sockets, shell, hash table, threading
├── Part 5 - Expert Systems & Application Development/         # Build systems and capstone work
│   └── 31_make_files_for_multi_file_projects/                 # Multi-file C project + Makefile
├── README.md
└── LICENSE
```

## Development Workflow

Common commands while working through lessons:

```bash
# Compile any single-file lesson
gcc -Wall -Wextra -std=c11 -o program '<lesson-file>.c'

# Run that lesson
./program

# Build the multi-file lesson with make
cd 'Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects'
make
./program
```

## Deployment and Operations

This repository is a learning course, not a deployable service. Operations work is limited to local compile/run workflows.

## Security and Reliability Notes

- Treat every lesson as local development code, not production-hardened software.
- Prefer compiling with warning flags enabled (`-Wall -Wextra`) on every exercise.
- For pointer and dynamic memory lessons, validate bounds and cleanup paths while experimenting.

## Documentation

| Path | Purpose |
|---|---|
| [Part 1 - The Beginner Path_ Core Concepts](Part%201%20-%20The%20Beginner%20Path_%20Core%20Concepts/) | Core syntax and first programs |
| [Part 2 - The Intermediate Path_ Deeper into C](Part%202%20-%20The%20Intermediate%20Path_%20Deeper%20into%20C/) | Pointers, memory, file I/O |
| [Part 3 - The Advanced Path_ Towards Mastery](Part%203%20-%20The%20Advanced%20Path_%20Towards%20Mastery/) | Larger C programs and advanced concepts |
| [Part 4 - The Expert Path_ Systems and Concurrency](Part%204%20-%20The%20Expert%20Path_%20Systems%20and%20Concurrency/) | Systems programming and concurrency |
| [Part 5 - Expert Systems & Application Development](Part%205%20-%20Expert%20Systems%20%26%20Application%20Development/) | Expert lessons and capstone track |

## Contributing

Open an issue or pull request with lesson fixes, typo corrections, or additional exercises. Keep changes aligned with the existing single-file teaching format.

## License

Licensed under the [MIT License](LICENSE).
