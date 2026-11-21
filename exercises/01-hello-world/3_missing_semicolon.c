/**
 * @file 3_missing_semicolon.c
 * @brief Lesson 1, Exercise 3: what you should have seen
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Delete the semicolon after the `printf` call and compile. Read the error
 * message: which line does it point at? Put the semicolon back.
 *
 * This file is the program with the semicolon back in place, so it compiles.
 * The comments describe what the compiler says without it.
 */

#include <stdio.h>

int main(void)
{
    printf("Hello, World!\n"); // <-- the semicolon the exercise removes

    return 0;
}

/*
 * WHAT THE COMPILER SAYS
 *
 * Without the semicolon, GCC stops with an error like this (the first
 * number is the line, the second is the column):
 *
 *   hello.c:5:30: error: expected ';' before 'return'
 *       5 |     printf("Hello, World!\n")
 *         |                              ^
 *         |                              ;
 *
 * Clang words it differently but means the same thing:
 *
 *   hello.c:5:30: error: expected ';' after expression
 *
 * HOW TO READ IT
 *
 * - It is an ERROR, not a warning: no executable is made. If an old
 *   executable is still in the directory, running it shows the old program.
 * - "expected ';' before 'return'" tells you how the compiler found out. It
 *   did not notice anything wrong at the end of the `printf` line; it only
 *   became confused when it reached the next word, `return`. Current
 *   compilers still point at the `printf` line and even show where the `;`
 *   belongs. Older ones point at the `return` line instead.
 * - That is the habit to take from this exercise: when the line an error
 *   names looks fine, look at the end of the line BEFORE it.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_missing_semicolon 3_missing_semicolon.c
 *   ./3_missing_semicolon
 */
