# Setting Up

Fifteen minutes, once. You will install a C compiler, get the lesson files and
run your first program from a terminal. The same guide is on the site at
<https://cfromthegroundup.com/setup>.

## What You Need

- A **C compiler**. Clang and GCC both work; on every system below the command
  is called `cc`.
- A **terminal**, the text window where you type commands. Every lesson is
  compiled and run from one.
- A **text editor** for reading and changing the code. Any editor works; VS
  Code is a good free choice.

Lessons 1 to 25 and 36 to 38 are portable C and run anywhere. Lessons 26 to
35 use POSIX
system calls or the ncurses library, so they need macOS, Linux, or Linux inside
Windows (WSL). The instructions below set all of that up.

## macOS

Open **Terminal** (in Applications, Utilities) and install Apple's command line
tools. They include the Clang compiler, `make` and ncurses:

```sh
xcode-select --install
```

A window asks you to confirm the download. When it finishes, check that the
compiler answers:

```sh
cc --version
```

On macOS, `gcc` is another name for Clang. That is fine: every lesson compiles
with either.

## Linux

Install the compiler, `make` and the ncurses headers with your package manager.

Debian, Ubuntu and Linux Mint:

```sh
sudo apt update
sudo apt install build-essential libncurses-dev
```

Fedora:

```sh
sudo dnf install gcc make ncurses-devel
```

Arch Linux:

```sh
sudo pacman -S base-devel ncurses
```

Then check that the compiler answers:

```sh
cc --version
```

## Windows (WSL)

On Windows, take the course inside the **Windows Subsystem for Linux**: a real
Ubuntu that runs next to Windows. This is the supported way to take the course
on Windows, and every lesson works there, including the POSIX and ncurses ones.
Open **PowerShell** as administrator and run:

```sh
wsl --install
```

Restart when asked, then open **Ubuntu** from the Start menu and choose a user
name and password. From then on you are on Linux: follow the Debian and Ubuntu
steps above.

Your Windows drives appear inside Ubuntu under `/mnt/` (for example
`/mnt/c/Users/you`), but keep the lessons in your Ubuntu home folder, where
compiling is faster. VS Code with its WSL extension can edit files there
directly.

## Using the Terminal

A few commands are all you need:

- `pwd` prints the folder you are in.
- `ls` lists the files in it.
- `cd folder` moves into a folder, and `cd ..` moves back up one.

Folder names with spaces must be quoted: `cd "Part 1 - The Beginner Path_ Core Concepts"`.
Press the Tab key to complete a name for you.

## Get the Lesson Files

Every lesson on the site has a **Download** button on each source file. To get
the whole course at once, clone the repository with git:

```sh
git clone https://github.com/dunamismax/c-from-the-ground-up.git
cd c-from-the-ground-up
```

The folder names contain spaces, so put quotes around them when you change into
one:

```sh
cd "Part 1 - The Beginner Path_ Core Concepts"
```

## Your First Program

In the folder that holds `1_hello_world.c`, compile it, then run the program
the compiler made:

```sh
cc -Wall -Wextra -std=c17 -o 1_hello_world 1_hello_world.c
./1_hello_world
```

If the terminal prints `Hello, World!`, you are ready. Head to
[Lesson 1: Hello, World!](chapters/01-hello-world.md) to learn what each line of
that program does.

## What the Flags Mean

- `cc` runs the C compiler.
- `-Wall -Wextra` turn on warnings. A warning is the compiler pointing at code
  that is legal but probably wrong. Read every one; the course's own code
  compiles with none.
- `-std=c17` picks the version of the C language standard to follow. This
  course teaches C17, which every current compiler supports.
- `-o name` names the program the compiler writes. Without it, you get `a.out`.
- `./name` runs a program from the current folder. The `./` is needed because
  the shell does not look for programs there by default.

## Troubleshooting

- **`cc: command not found`**: the compiler is not installed yet. Repeat the
  steps for your system above.
- **`No such file or directory`**: you are not in the folder that holds the
  lesson file. Run `ls` to see what is there, and `cd` into the right folder.
- **``undefined reference to `pthread_create` ``** (or to `sqrt`): the lesson
  needs a library. Use the exact command in its "How to Compile and Run"
  section; it includes `-pthread`, `-lm` or `-lncurses` where needed.
- **`cannot find -lncurses`** or **`ncurses.h: No such file`**: install the
  ncurses development package (`libncurses-dev` or `ncurses-devel`). Some
  systems call the library `ncursesw`: try `-lncursesw`.
- **The program waits and nothing happens**: it is probably waiting for you to
  type something. Press Ctrl+C to stop any program.
