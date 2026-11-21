/**
 * @file 2_three_lines.c
 * @brief Lesson 1, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Print three lines with three `printf` calls, then print the same three
 * lines with one `printf` that contains three `\n`s.
 */

#include <stdio.h>

int main(void)
{
    // First way: one `printf` call per line. Each string ends with `\n`,
    // which moves the cursor to the start of the next line.
    printf("Roses are red,\n");
    printf("Violets are blue,\n");
    printf("C is a language.\n");

    // Second way: one call. `printf` does not start a new line on its own;
    // a new line begins only where the string contains `\n`. So one string
    // with three `\n`s prints three lines.
    printf("Roses are red,\nViolets are blue,\nC is a language.\n");

    return 0;
}

/*
 * Both ways print exactly the same characters. Try removing a `\n`: the two
 * lines around it run together, because nothing told the console to move
 * down.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_three_lines 2_three_lines.c
 *   ./2_three_lines
 */
