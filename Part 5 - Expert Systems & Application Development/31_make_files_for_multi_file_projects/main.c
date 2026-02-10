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