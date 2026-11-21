/**
 * @file 1_temperature.c
 * @brief Lesson 5, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Read a temperature in Celsius and print "freezing", "cold", "mild" or "hot"
 * for ranges you choose.
 */

#include <stdio.h>

int main(void)
{
    // A temperature can have a decimal part (21.5), so it is a `double`.
    // Remember: `scanf` reads a `double` with `%lf`.
    double celsius = 0.0;

    printf("Enter a temperature in Celsius: ");
    if (scanf("%lf", &celsius) != 1)
    {
        printf("That was not a number. Exiting.\n");
        return 1;
    }

    // The ranges chosen here: below 0 is freezing, 0 up to 10 is cold,
    // 10 up to 25 is mild, and 25 or more is hot.
    //
    // The chain is checked from the top and stops at the first condition
    // that is true. So when the second test runs, we already know the
    // temperature is NOT below 0, and `celsius < 10` is enough: there is no
    // need to write `celsius >= 0 && celsius < 10`.
    if (celsius < 0)
    {
        printf("%.1f degrees is freezing.\n", celsius);
    }
    else if (celsius < 10)
    {
        printf("%.1f degrees is cold.\n", celsius);
    }
    else if (celsius < 25)
    {
        printf("%.1f degrees is mild.\n", celsius);
    }
    else
    {
        printf("%.1f degrees is hot.\n", celsius);
    }

    return 0;
}

/*
 * TEST THE EDGES
 *
 * Bugs in a chain like this hide at the boundaries, so test the values
 * where one range ends and the next begins: -0.5, 0, 9.9, 10, 24.9 and 25.
 * Decide what you want 0 and 10 to print, then check that they do.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_temperature 1_temperature.c
 *   ./1_temperature
 */
