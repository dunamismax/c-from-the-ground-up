/**
 * @file 1_five_results.c
 * @brief Lesson 4, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Read two integers with `scanf` and print their sum, difference, product,
 * integer quotient and remainder.
 */

#include <stdio.h>

int main(void)
{
    int a = 0;
    int b = 0;

    printf("Enter the first whole number: ");
    if (scanf("%d", &a) != 1)
    {
        printf("Could not read a whole number. Exiting.\n");
        return 1;
    }

    printf("Enter the second whole number: ");
    if (scanf("%d", &b) != 1)
    {
        printf("Could not read a whole number. Exiting.\n");
        return 1;
    }

    printf("%d + %d = %d\n", a, b, a + b);
    printf("%d - %d = %d\n", a, b, a - b);
    printf("%d * %d = %d\n", a, b, a * b);

    // Dividing an integer by zero is an error the language does not define:
    // on most computers the program simply crashes. `/` and `%` both divide,
    // so both need this check. It has the same shape as the `scanf` check
    // above: "if this is true, explain and stop". Lesson 5 covers `if`.
    if (b == 0)
    {
        printf("Cannot divide by zero, so there is no quotient or remainder.\n");
        return 1;
    }

    printf("%d / %d = %d\n", a, b, a / b);
    printf("%d %% %d = %d\n", a, b, a % b);

    return 0;
}

/*
 * THINGS TO TRY
 *
 * - 17 and 5: the quotient is 3 and the remainder is 2, because
 *   17 = 3 * 5 + 2. The quotient and remainder always fit together this way.
 * - 5 and 17: the quotient is 0 and the remainder is 5.
 * - 7 and 0: the program explains instead of crashing.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_five_results 1_five_results.c
 *   ./1_five_results
 */
