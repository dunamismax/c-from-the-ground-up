/**
 * @file 2_decimal_places.c
 * @brief Lesson 2, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Print `item_price` with `%.0f`, `%.1f` and `%.3f`. What does the number
 * after the dot control?
 */

#include <stdio.h>

int main(void)
{
    double item_price = 19.99;

    printf("With %%.0f: %.0f\n", item_price); // 20
    printf("With %%.1f: %.1f\n", item_price); // 20.0
    printf("With %%.3f: %.3f\n", item_price); // 19.990

    return 0;
}

/*
 * THE ANSWER
 *
 * The number after the dot is the PRECISION: how many digits `printf` prints
 * after the decimal point.
 *
 * - `%.0f` prints no decimals at all, not even the point: 20.
 * - `%.1f` prints one: 20.0.
 * - `%.3f` prints three, adding a zero to fill the space: 19.990.
 *
 * Notice that 19.99 became 20 and 20.0. `printf` ROUNDS to the nearest value
 * it can show; it does not simply cut digits off. The variable itself is
 * not changed: `item_price` still holds 19.99, and only the printed text is
 * rounded.
 *
 * (`%%` in the format string prints one real percent sign. Lesson 4 uses it
 * too.)
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_decimal_places 2_decimal_places.c
 *   ./2_decimal_places
 */
