# Makefiles for Multi-File Projects

Welcome to a pivotal moment in your C journey. Until now, we have worked with
single source files. While great for learning, real-world applications are almost
always split into multiple files for organization, reusability, and clarity.

Typing out long `gcc` commands for every file becomes tedious and error-prone.
Imagine a project with 50 files. This is where the `make` utility comes in.

## What is a Makefile?

A `make` program reads a special file, by default named `Makefile`, to understand
how to build your project. A Makefile contains a set of **rules**. Each rule has
three parts: a **target**, its **dependencies**, and a **recipe**.

```text
target: dependencies
    <tab>recipe (the command to run)
```

- **Target:** The file we want to build (e.g., an executable or an object file).
- **Dependencies:** The files the target needs in order to be built.
- **Recipe:** The command(s) to execute to create the target.

**Important:** The recipe lines must start with a TAB character, not spaces.

## The Project Structure

This lesson is intentionally split across multiple files to demonstrate
how different C files work together:

1. `main.c` -- The entry point of the program. Contains the `main` function.
2. `helper.h` -- The header file that *declares* the helper functions.
3. `helper.c` -- The source file that *defines* the helper functions.
4. `Makefile` -- The instruction manual for the compiler.

## helper.h

<!-- source: Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects/helper.h -->
```c
/**
 * @file helper.h
 * @brief Part 5, Lesson 31: Header File for the Helper Module
 * @author dunamismax
 * @date 06-15-2025
 *
 * This is a HEADER FILE. Its purpose is to declare the functions and types
 * that another part of the program (in this case, `helper.c`) provides.
 * It acts as a public "interface" or "contract".
 */

/*
 * =====================================================================================
 * |                            - LESSON: HEADER FILES -                               |
 * =====================================================================================
 *
 * WHAT IS A HEADER FILE?
 * In a multi-file project, we need a way for one `.c` file to know about the
 * functions available in another `.c` file. A HEADER FILE (with a `.h` extension)
 * serves this purpose. It contains the DECLARATIONS of functions, but not their
 * actual code (the DEFINITIONS).
 *
 * Any `.c` file that wants to use these functions can simply `#include` this
 * header file.
 *
 * --- The Include Guard ---
 *
 * What happens if a header file gets included more than once in the same
 * compilation process? Declaring a function twice is harmless, but headers
 * often also DEFINE things, such as a `struct` type, and defining those twice
 * is an error ("redefinition of 'struct ...'").
 * To prevent this, we use an INCLUDE GUARD. It's a standard preprocessor trick.
 *
 * `#ifndef HELPER_H`   // "If HELPER_H is NOT defined..."
 * `#define HELPER_H`   // "...then define it..."
 *
 *  // ... all the content of the header file goes here ...
 *
 * `#endif`             // "...and this is the end of the 'if' block."
 *
 * The first time the compiler sees this file, `HELPER_H` is not defined, so it
 * processes everything inside. It also defines `HELPER_H`. The second time it
 * sees this file, `HELPER_H` *is* defined, so the preprocessor skips everything
 * between `#ifndef` and `#endif`.
 *
 * The name `HELPER_H` should be unique to the file. A common convention is to
 * use the filename in all caps, replacing the `.` with an `_`.
 */

#ifndef HELPER_H
#define HELPER_H

// --- Function Prototypes ---

// This is a FUNCTION PROTOTYPE or FUNCTION DECLARATION.
// It tells the compiler everything it needs to know to *call* the function:
// its return type, its name, and the types of its parameters.
// The actual code for this function is in `helper.c`.
void say_hello_from_helper(void);

#endif // HELPER_H
```

## helper.c

<!-- source: Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects/helper.c -->
```c
/**
 * @file helper.c
 * @brief Part 5, Lesson 31: Implementation File for the Helper Module
 * @author dunamismax
 * @date 06-15-2025
 *
 * This is a SOURCE FILE that provides the implementation for the
 * functions declared in `helper.h`.
 */

/*
 * =====================================================================================
 * |                          - LESSON: SOURCE FILES -                                 |
 * =====================================================================================
 *
 * WHAT IS A SOURCE IMPLEMENTATION FILE?
 * This `.c` file contains the actual code, the DEFINITIONS, for the functions
 * that were promised (declared) in the corresponding header file (`helper.h`).
 *
 * By separating the declaration (`.h`) from the definition (`.c`), we create
 * modular, reusable code. Another programmer could use our `helper` module by
 * just reading `helper.h`; they wouldn't need to know the details of our code here.
 *
 * --- Including Local Headers ---
 * Notice we use `#include "helper.h"` with double quotes. This is a signal
 * to the compiler to look for the file in the directory that holds THIS source
 * file first, before searching the system's standard library paths.
 *
 *  - `#include <stdio.h>`: Use angle brackets for system libraries.
 *  - `#include "helper.h"`: Use double quotes for your own project's header files.
 */

#include <stdio.h>
#include "helper.h" // We need to include our own header file.

/**
 * @brief Prints a message to demonstrate a function call from a separate file.
 *
 * This is the DEFINITION of the function that was declared in `helper.h`.
 * It contains the actual block of code that will be executed when the
 * function is called.
 */
void say_hello_from_helper(void)
{
    printf("Hello from the helper module! This message comes from helper.c.\n");
}
```

## main.c

<!-- source: Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects/main.c -->
```c
/**
 * @file main.c
 * @brief Part 5, Lesson 31: Using a Makefile and Multiple Source Files
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file demonstrates how to use a function from a separate module
 * (`helper.c`) by including its header file (`helper.h`).
 */

/*
 * =====================================================================================
 * |                      - LESSON: PUTTING IT ALL TOGETHER -                          |
 * =====================================================================================
 *
 * Welcome to our first multi-file project!
 *
 * The goal of this lesson is to see how different C files work together.
 *
 * 1. `main.c` (This file): This is the entry point of our program. It contains
 *    the `main` function. It needs to call a function from our "helper" module.
 *
 * 2. `helper.h` (Header file): This file *declares* the functions our helper
 *    module provides. By including it here (`#include "helper.h"`), we tell
 *    the compiler "Trust me, a function named `say_hello_from_helper` exists
 *    somewhere. Here's what it looks like."
 *
 * 3. `helper.c` (Source file): This file *defines* the `say_hello_from_helper`
 *    function, providing the actual code for it.
 *
 * 4. `Makefile`: This is the instruction manual for the compiler. It tells `gcc`
 *    how to compile `main.c` and `helper.c` separately into "object files",
 *    and then how to "link" them together into a single, final executable.
 */

#include <stdio.h>
#include "helper.h" // Include the declarations from our helper module.

/**
 * @brief The main entry point of the program.
 *
 * This program demonstrates linking multiple source files. It calls a function
 * defined in `helper.c`.
 *
 * @return 0 on successful execution.
 */
int main(void)
{
    printf("Message from main.c: Calling a function from another file...\n");

    // This function call is possible because we included `helper.h`.
    // The compiler knows the function's "signature" from the header.
    // The LINKER (part of the compilation process guided by the Makefile)
    // is responsible for connecting this call to the actual function code
    // in `helper.c`.
    say_hello_from_helper();

    printf("Message from main.c: The helper function has completed.\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                 - COMPILATION -                                   |
 * =====================================================================================
 *
 * NOTE: Do NOT compile this file directly with a simple `gcc` command.
 *
 * This is a multi-file project. To compile and run it, you must use the
 * `make` utility with the provided `Makefile`.
 *
 * Open your terminal in this directory and run:
 *
 *   `make`
 *
 * Then run the resulting executable:
 *
 *   `./31_project_main`
 *
 * For a detailed explanation, read the comments in the `Makefile`.
 */
```

## The Makefile

The Makefile ties it all together. It uses variables, rules, and phony targets
to automate the build process.

<!-- source: Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects/Makefile -->
```makefile
# Makefile
# @brief Part 5, Lesson 31: Makefiles for Multi-File Projects
# @author dunamismax
# @date 06-15-2025
#
# This file is the lesson on Makefiles. Read the comments from top to bottom
# to learn how to automate the compilation of complex C projects.

# =====================================================================================
# |                                   - LESSON START -                                  |
# =====================================================================================
#
# Welcome to a pivotal moment in your C journey! Until now, we've worked with
# single source files. While great for learning, real-world applications are almost
# always split into multiple files for organization, reusability, and clarity.
#
# Typing out long `gcc` commands for every file becomes tedious and error-prone.
# Imagine a project with 50 files! This is where the `make` utility comes in.
#
# WHAT IS A MAKEFILE?
# A `make` program reads a special file, by default named `Makefile`, to understand
# how to build your project. A Makefile contains a set of RULES. Each rule has
# three parts: a TARGET, its DEPENDENCIES, and a RECIPE.
#
#   target: dependencies
#       <tab>recipe (the command to run)
#
# - TARGET: The file we want to build (e.g., an executable or an object file).
# - DEPENDENCIES: The files the target needs in order to be built.
# - RECIPE: The command(s) to execute to create the target.
#
# IMPORTANT: The lines with recipes MUST start with a TAB character, not spaces!

# --- Part 1: Using Variables ---
#
# To make our Makefile clean and easy to modify, we use VARIABLES.
# It's convention to use all caps for variable names.

# The C compiler we want to use.
CC = gcc

# The flags we pass to the compiler. These are the same flags we've been using.
# CFLAGS stands for "C Compiler Flags".
CFLAGS = -Wall -Wextra -std=c11

# The name of the final executable file we want to build.
TARGET = 31_project_main

# --- Part 2: Defining the Rules ---
#
# Now we define the rules for building our project. `make` reads these rules
# and figures out what needs to be compiled or re-compiled.

# The first rule in a Makefile is the default rule. When you just type `make`
# in the terminal, this is the target that `make` will try to build. We want
# it to build our entire project.
#
# This rule says: To make the TARGET (`31_project_main`), you first need
# `main.o` and `helper.o`.
# The recipe then links these two object files together to create the final executable.
$(TARGET): main.o helper.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o helper.o

# This rule says: To make `main.o`, you need `main.c` and `helper.h`.
# If `main.c` or `helper.h` has changed since the last time `main.o` was built,
# `make` will run the recipe. The `-c` flag tells GCC to COMPILE ONLY, creating
# an "object file" (`.o`) without linking.
main.o: main.c helper.h
	$(CC) $(CFLAGS) -c main.c

# This is the rule for our helper module.
# To make `helper.o`, you need `helper.c` and `helper.h`.
helper.o: helper.c helper.h
	$(CC) $(CFLAGS) -c helper.c

# --- Part 3: Phony Targets ---
#
# Sometimes we want a target that isn't an actual file. For example, a "clean"
# target to delete all the compiled files. These are called PHONY targets.
# We declare them with `.PHONY` to tell `make` that this target name doesn't
# correspond to a file it should expect to exist.

.PHONY: all clean

# A common convention is to have an `all` target that is just another name for
# the default target. It doesn't do anything new but provides a clear command.
all: $(TARGET)

# This rule cleans up our directory.
# The `rm` command removes files. The `-f` flag means "force", so it won't
# complain if the files don't exist.
# We remove the object files (`*.o`) and the final executable (`$(TARGET)`).
clean:
	rm -f $(TARGET) *.o

# =====================================================================================
# |                                    - LESSON END -                                   |
# =====================================================================================
#
# HOW TO USE THIS MAKEFILE:
#
# 1. Open a terminal in the directory containing this Makefile and the .c/.h files.
#
# 2. To build the entire project:
#    `make`
#    (or `make all`)
#
#    You will see the commands from the recipes being executed. An executable
#    named `31_project_main` will be created.
#
# 3. To run the program:
#    - On Linux/macOS:   `./31_project_main`
#    - On Windows:       `31_project_main.exe`
#
# 4. To clean up all generated files (the executable and object files):
#    `make clean`
#
# Try it! Run `make`, then run the program. Then run `make` again. Notice how
# `make` says everything is "up to date"? Now, modify `helper.c` (e.g., add a
# comment) and run `make` again. See how it only re-compiles what's necessary?
# This is the power of `make`!
```

## How to Compile and Run

Build the project:

```sh
make
```

Run the program:

```sh
./31_project_main
```

Clean up generated files:

```sh
make clean
```

Try it: run `make`, then run the program. Then run `make` again. Notice how
`make` says everything is "up to date." Now modify `helper.c` (for example, add
a comment) and run `make` again. See how it only re-compiles what changed?
That is the power of `make`.

## Key Takeaways

- Real-world C projects are split into multiple source files for organization.
- **Header files** (`.h`) declare function signatures so other files can use them.
- **Source files** (`.c`) define the actual function implementations.
- The `#include "header.h"` directive (with quotes) includes your own headers.
- A **Makefile** automates the compile-and-link process so you do not have to type
  long `gcc` commands by hand.
- `make` only re-compiles files that have changed, saving time on large projects.
- Use `make clean` to remove generated build artifacts.
