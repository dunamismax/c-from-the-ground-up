# Hello, World!

> **What you'll learn:** how a C program goes from source code to a running program, and what `#include`, `main`, `printf` and `return` do.
>
> **Before you start:** Nothing. Install a compiler with the setup guide and you are ready.

Welcome to your first lesson in C! The journey to mastering C is challenging
but incredibly rewarding. It all begins with this single step.

## What Is a Compiler?

A COMPILER is a special program that translates the human-readable C code you
write into the machine code that your computer can execute. The commands in this
course call it `cc`, the usual name for your system's C compiler: GCC (the GNU
Compiler Collection) on most Linux systems, and Clang on macOS. Both work the
same way for these lessons.

## The Program, Line by Line

The whole program is only a few lines long. It starts with this one:

```c
#include <stdio.h>
```


The `#include` is a PREPROCESSOR DIRECTIVE. It's a command that runs
*before* the main compilation process starts. It tells the compiler to find
the file specified in the angle brackets `< >` and copy its entire content
into the program at that spot.

We are including `stdio.h`, which stands for "Standard Input/Output Header".
This header file contains declarations for essential functions used for
input and output operations, including the `printf` function we will use below.

Next comes the MAIN FUNCTION, which holds the rest of the program:

```c
int main(void)
{
    printf("Hello, World!\n");
    return 0;
}
```

Every complete C program MUST have exactly one
function named `main`. The execution of the program officially begins at the
top of the `main` function. The curly braces `{` and `}` mark where the
function's BODY, its block of code, begins and ends.

Let's break down its signature: `int main(void)`

- `int`: This is the RETURN TYPE. It specifies that the `main` function will
  "return" an integer value to the operating system when it finishes. This
  integer is an "exit code" that signals whether the program ran successfully.

- `main`: This is the required NAME for the starting function.

- `(void)`: This specifies the function's PARAMETERS (the inputs it takes).
  `void` is a special keyword that means "nothing". So, `(void)` tells us
  that this function takes no arguments or inputs.

`printf()` is a FUNCTION that "prints formatted" output to your console
(the text window where you'll run this program). This function is made
available to us because we included `<stdio.h>`.

The text inside the double quotes, `"Hello, World!\n"`, is a STRING LITERAL.
This is the data we are passing as an argument to the `printf` function.

The `\n` at the end is a special "escape sequence" that represents a NEWLINE
character. It tells the console to move the cursor to the next line after
printing "Hello, World!".

Finally, the semicolon `;` at the end marks the end of a C STATEMENT.
Most lines of executable code in C must end with a semicolon.

The `return` statement ends the execution of the `main` function. It sends
a value back to the operating system. By convention, `return 0;` signals
that the program executed successfully without any errors.

## Full Source

<!-- source: Part 1 - The Beginner Path_ Core Concepts/1_hello_world.c -->
```c
/**
 * @file 1_hello_world.c
 * @brief Part 1, Lesson 1: Hello, World!
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file is your very first C program and lesson.
 * The lesson is taught through the comments in this file. Read them
 * from top to bottom to understand what's happening.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Welcome to your first lesson in C! The journey to mastering C is challenging
 * but incredibly rewarding. It all begins with this single step.
 *
 * WHAT IS A COMPILER?
 * A COMPILER is a special program that translates the human-readable C code you
 * write into the machine code that your computer can execute. For this course,
 * we will use GCC (GNU Compiler Collection).
 */

// --- Part 1: Preprocessor Directives ---

/*
 * The `#include` is a PREPROCESSOR DIRECTIVE. It's a command that runs
 * *before* the main compilation process starts. It tells the compiler to find
 * the file specified in the angle brackets `< >` and copy its entire content
 * into this file right here.
 *
 * We are including `stdio.h`, which stands for "Standard Input/Output Header".
 * This header file contains declarations for essential functions used for
 * input and output operations, including the `printf` function we will use below.
 */
#include <stdio.h>

// --- Part 2: The Main Function ---

/*
 * This is the MAIN FUNCTION. Every complete C program MUST have exactly one
 * function named `main`. The execution of the program officially begins at the
 * top of the `main` function.
 *
 * Let's break down its signature: `int main(void)`
 *
 * - `int`: This is the RETURN TYPE. It specifies that the `main` function will
 *   "return" an integer value to the operating system when it finishes. This
 *   integer is an "exit code" that signals whether the program ran successfully.
 *
 * - `main`: This is the required NAME for the starting function.
 *
 * - `(void)`: This specifies the function's PARAMETERS (the inputs it takes).
 *   `void` is a special keyword that means "nothing". So, `(void)` tells us
 *   that this function takes no arguments or inputs.
 */
int main(void)
{ // The opening brace `{` marks the beginning of the function's code block (its body).

    // --- Part 3: Statements and Functions ---

    /*
     * `printf()` is a FUNCTION that "prints formatted" output to your console
     * (the text window where you'll run this program). This function is made
     * available to us because we included `<stdio.h>`.
     *
     * The text inside the double quotes, `"Hello, World!\n"`, is a STRING LITERAL.
     * This is the data we are passing as an argument to the `printf` function.
     *
     * The `\n` at the end is a special "escape sequence" that represents a NEWLINE
     * character. It tells the console to move the cursor to the next line after
     * printing "Hello, World!".
     *
     * Finally, the semicolon `;` at the end marks the end of a C STATEMENT.
     * Most lines of executable code in C must end with a semicolon.
     */
    printf("Hello, World!\n");

    /*
     * The `return` statement ends the execution of the `main` function. It sends
     * a value back to the operating system. By convention, `return 0;` signals
     * that the program executed successfully without any errors.
     */
    return 0;

} // The closing brace `}` marks the end of the function's body.

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Congratulations on writing your first C program!
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal. On Windows, use the Ubuntu terminal of WSL (the Windows
 *    Subsystem for Linux), which is how this course runs on Windows.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to translate this source code into an executable program.
 *    The command below does the following:
 *    - `gcc`: Invokes the compiler.
 *    - `-Wall -Wextra`: Enables all and extra warning flags to catch potential errors.
 *    - `-std=c17`: Asks for the C17 standard of the language. C has been revised
 *      several times (C89, C99, C11, C17, C23); C17 is supported by every current
 *      compiler, so it is the version this course teaches.
 *    - `-o 1_hello_world`: Specifies the output file name should be `1_hello_world`.
 *    - `1_hello_world.c`: Specifies the input source file to compile.
 *
 *    `gcc -Wall -Wextra -std=c17 -o 1_hello_world 1_hello_world.c`
 *
 * 4. Run the newly created executable:
 *    - On Linux/macOS:   `./1_hello_world`
 *    - On Windows (WSL): `./1_hello_world`
 *
 * You should see "Hello, World!" printed to your console.
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c17 -o 1_hello_world 1_hello_world.c
./1_hello_world
```

## Try It Yourself

Practise what this lesson taught. Change a copy of the lesson file, or
start a new one.

1. Change the message to greet yourself by name, then compile and run it again.
2. Print three lines with three `printf` calls, then print the same three lines with one `printf` and two `\n`s.
3. Delete the semicolon after the `printf` call and compile. Read the error message: which line does it point at? Put the semicolon back.
