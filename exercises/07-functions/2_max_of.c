/**
 * @file 2_max_of.c
 * @brief Lesson 7, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Write `int max_of(int a, int b)` and use it twice to find the largest of
 * three numbers.
 */

#include <stdio.h>

// --- Function Prototypes ---
int max_of(int a, int b);

int main(void)
{
    int first = 12;
    int second = 45;
    int third = 30;

    // Step by step: find the larger of the first two, then compare that
    // winner with the third.
    int larger_of_two = max_of(first, second);
    int largest = max_of(larger_of_two, third);
    printf("The largest of %d, %d and %d is %d.\n", first, second, third, largest);

    // The same thing in one line. The inner call runs first, and the value
    // it returns becomes the first argument of the outer call.
    printf("In one line: %d\n", max_of(max_of(first, second), third));

    // It also works when numbers are equal.
    printf("The largest of 7, 7 and 2 is %d.\n", max_of(max_of(7, 7), 2));

    return 0;
}

/**
 * @brief Returns the larger of two integers (either one if they are equal).
 */
int max_of(int a, int b)
{
    if (a > b)
    {
        return a;
    }

    // Reached when b is larger, and also when the two are equal, where
    // returning either one is correct.
    return b;
}

/*
 * A function that only knows about TWO numbers solved a problem about
 * THREE, because one call's result can be another call's argument. The same
 * idea finds the largest of four: `max_of(max_of(a, b), max_of(c, d))`.
 *
 * Compare this with Lesson 5, Exercise 3, where the comparison was written
 * out inside `main`. Here it is written once, has a name, and can be tested
 * on its own.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_max_of 2_max_of.c
 *   ./2_max_of
 */
