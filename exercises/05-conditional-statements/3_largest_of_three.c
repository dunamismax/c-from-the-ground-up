/**
 * @file 3_largest_of_three.c
 * @brief Lesson 5, Exercise 3: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Read three integers and print the largest. Make sure it works when two of
 * them are equal.
 */

#include <stdio.h>

int main(void)
{
    int first = 0;
    int second = 0;
    int third = 0;

    printf("Enter the first whole number: ");
    if (scanf("%d", &first) != 1)
    {
        printf("That was not a whole number. Exiting.\n");
        return 1;
    }

    printf("Enter the second whole number: ");
    if (scanf("%d", &second) != 1)
    {
        printf("That was not a whole number. Exiting.\n");
        return 1;
    }

    printf("Enter the third whole number: ");
    if (scanf("%d", &third) != 1)
    {
        printf("That was not a whole number. Exiting.\n");
        return 1;
    }

    // Keep the best candidate so far in its own variable. Start by assuming
    // the first number is the largest, then let each of the others replace
    // it if it is bigger.
    int largest = first;

    if (second > largest)
    {
        largest = second;
    }

    if (third > largest)
    {
        largest = third;
    }

    printf("The largest is %d.\n", largest);

    return 0;
}

/*
 * WHY THIS HANDLES EQUAL NUMBERS
 *
 * These are two separate `if` statements, not an `if-else` chain, because
 * both questions must always be asked. When two numbers are equal, `>` is
 * false, nothing is replaced, and `largest` already holds that value. Try
 * 5 5 3, 3 5 5, 5 3 5 and 4 4 4.
 *
 * The other common solution compares everything with everything:
 *
 *   if (first >= second && first >= third) ... else if (second >= third) ...
 *
 * It works, but only with `>=`. Written with `>`, the input 5 5 3 makes the
 * first test false (5 > 5 is false) and the program can pick the wrong
 * branch. The "best so far" pattern above has no such trap, and it is the
 * one you will reuse in Lesson 8 to find the largest value in an array.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_largest_of_three 3_largest_of_three.c
 *   ./3_largest_of_three
 */
