/**
 * @file 2_what_scanf_returns.c
 * @brief Lesson 3, Exercise 2: the answer, and a program that shows it
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Run the program and type `abc` at the first prompt, then run it again and
 * press Ctrl+D. Which message do you get each time, and why?
 *
 * THE ANSWER:
 * You get the same message both times: "Could not read a whole number.
 * Exiting." But `scanf` failed for two different reasons, and it reports
 * them with two different return values. This program prints that value so
 * you can see it.
 */

#include <stdio.h>

int main(void)
{
    int user_age = 0;

    printf("Please enter your age (as a whole number): ");

    // Keep what `scanf` returns in a variable instead of comparing it
    // straight away.
    int items_read = scanf("%d", &user_age);

    printf("\nscanf returned %d.\n", items_read);

    if (items_read != 1)
    {
        printf("Could not read a whole number. Exiting.\n");
        return 1;
    }

    printf("Thank you. You entered %d.\n", user_age);

    return 0;
}

/*
 * RUN IT THREE TIMES
 *
 * - Type `42`: scanf returned 1. One item was read and stored.
 * - Type `abc`: scanf returned 0. There was input, but it did not look like
 *   a whole number, so nothing was stored and `user_age` is still 0.
 * - Press Ctrl+D (Ctrl+Z then Enter on Windows): scanf returned -1. That is
 *   the value of `EOF` on your system. There was no input at all.
 *
 * The lesson's check, `!= 1`, is true for both 0 and -1. That is why one
 * `if` catches both failures, and why you see the same message.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_what_scanf_returns 2_what_scanf_returns.c
 *   ./2_what_scanf_returns
 */
