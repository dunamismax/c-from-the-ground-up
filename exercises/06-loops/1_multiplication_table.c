/**
 * @file 1_multiplication_table.c
 * @brief Lesson 6, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Print the multiplication table for a number the user enters, from 1 times
 * to 10 times.
 */

#include <stdio.h>

int main(void)
{
    int number = 0;

    printf("Which multiplication table would you like? ");
    if (scanf("%d", &number) != 1)
    {
        printf("That was not a whole number. Exiting.\n");
        return 1;
    }

    // We know in advance how many times to repeat (10), so `for` is the
    // right loop. The counter `i` is also the number we multiply by.
    // `i <= 10` (not `i < 10`) makes the last line "10 times".
    for (int i = 1; i <= 10; i++)
    {
        // `%2d` prints the number in a field at least 2 characters wide, so
        // the single digits line up with the 10.
        printf("%2d x %d = %d\n", i, number, i * number);
    }

    return 0;
}

/*
 * A loop that runs one time too many or too few is called an OFF-BY-ONE
 * error, and it is the most common loop bug there is. Check the first and
 * last lines of the output: they should be "1 x" and "10 x".
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_multiplication_table 1_multiplication_table.c
 *   ./1_multiplication_table
 */
