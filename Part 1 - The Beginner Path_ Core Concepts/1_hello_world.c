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
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to translate this source code into an executable program.
 *    The command below does the following:
 *    - `gcc`: Invokes the compiler.
 *    - `-Wall -Wextra`: Enables all and extra warning flags to catch potential errors.
 *    - `-std=c11`: Specifies that we are using the C11 standard of the language.
 *    - `-o 1_hello_world`: Specifies the output file name should be `HelloWorld`.
 *    - `1_hello_world.c`: Specifies the input source file to compile.
 *
 *    `gcc -Wall -Wextra -std=c11 -o HelloWorld HelloWorld.c`
 *
 * 4. Run the newly created executable:
 *    - On Linux/macOS:   `./1_hello_world`
 *    - On Windows:       `1_hello_world.exe`
 *
 * You should see "Hello, World!" printed to your console.
 */