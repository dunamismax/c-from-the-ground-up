/**
 * @file 2_reverse_order.c
 * @brief Lesson 8, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Read 5 integers from the user into an array, then print them in reverse
 * order.
 */

#include <stdio.h>

int main(void)
{
    int numbers[5] = {0, 0, 0, 0, 0};

    // First loop: fill the array from index 0 up to index 4.
    for (int i = 0; i < 5; i++)
    {
        printf("Enter number %d of 5: ", i + 1);

        // An array element is a variable like any other, so `scanf` needs
        // its address: `&numbers[i]`.
        if (scanf("%d", &numbers[i]) != 1)
        {
            printf("That was not a whole number. Exiting.\n");
            return 1;
        }
    }

    // Second loop: walk the indices backwards, from 4 down to 0.
    // - It starts at 4, the LAST valid index, not at 5.
    // - It continues while `i >= 0`, so index 0 is included.
    printf("In reverse order:");
    for (int i = 4; i >= 0; i--)
    {
        printf(" %d", numbers[i]);
    }
    printf("\n");

    return 0;
}

/*
 * The array itself is not changed; only the order we visit it in is. Both
 * mistakes that are easy to make here are off-by-one errors: starting at
 * `numbers[5]` reads past the end of the array, and stopping at `i > 0`
 * silently skips the first number. Enter 1 2 3 4 5 and check that you see
 * 5 4 3 2 1, with both the 5 and the 1.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_reverse_order 2_reverse_order.c
 *   ./2_reverse_order
 */
