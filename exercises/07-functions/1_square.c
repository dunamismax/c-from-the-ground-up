/**
 * @file 1_square.c
 * @brief Lesson 7, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Write `int square(int n)` and use it to print the squares of 1 to 10.
 */

#include <stdio.h>

// --- Function Prototypes ---
int square(int n);

int main(void)
{
    for (int i = 1; i <= 10; i++)
    {
        // The call `square(i)` is replaced by the value the function
        // returns, so it can be used directly as a `printf` argument.
        printf("%2d squared is %3d\n", i, square(i));
    }

    return 0;
}

/**
 * @brief Returns n multiplied by itself.
 */
int square(int n)
{
    // `n` is the function's own copy of the argument. On the first call it
    // holds 1, on the next 2, and so on; `main`'s `i` is never touched.
    return n * n;
}

/*
 * The loop and the calculation are now separate jobs: `main` decides WHICH
 * numbers to square, and `square` knows HOW. To print the squares of 1 to
 * 20 you change only the loop; to print cubes you change only the function.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_square 1_square.c
 *   ./1_square
 */
