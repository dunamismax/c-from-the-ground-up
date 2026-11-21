/**
 * @file 2_running_total.c
 * @brief Lesson 6, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Use a `while` loop to add up numbers the user types until they enter 0,
 * then print the total.
 */

#include <stdio.h>

int main(void)
{
    int total = 0;

    // The loop runs while `number` is not 0, and the condition is checked
    // BEFORE the first pass. So `number` must start as anything except 0,
    // or the loop would never run at all.
    int number = 1;

    while (number != 0)
    {
        printf("Enter a number (0 to stop): ");
        int items_read = scanf("%d", &number);

        if (items_read == 1)
        {
            // Adding the final 0 changes nothing, so it needs no special case.
            total = total + number;
        }
        else if (items_read == EOF)
        {
            // No input is left. Set the value that ends the loop, as the
            // lesson's menu does.
            printf("\nNo more input.\n");
            number = 0;
        }
        else
        {
            // Not a number. The bad text is still in the input buffer, so
            // read and discard it up to the end of the line, or the next
            // `scanf` would fail on it again, forever.
            printf("That was not a number. Please try again.\n");
            int leftover = getchar();
            while (leftover != '\n' && leftover != EOF)
            {
                leftover = getchar();
            }
        }
    }

    printf("The total is %d.\n", total);

    return 0;
}

/*
 * THINGS TO TRY
 *
 * - 5, 10, -3, 0 prints 12.
 * - 0 straight away prints 0: the loop body runs once and adds nothing.
 * - 5, abc, 10, 0 prints 15: the bad line is skipped and `number` keeps its
 *   old value (5, not 0), so the loop carries on.
 * - 5, then Ctrl+D prints 5.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_running_total 2_running_total.c
 *   ./2_running_total
 */
