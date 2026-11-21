/**
 * @file 3_leap_year.c
 * @brief Lesson 4, Exercise 3: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Write an expression that is 1 when an integer `year` is a leap year
 * (divisible by 4, and not by 100 unless also by 400), and test it on 1900,
 * 2000 and 2024.
 */

#include <stdio.h>

int main(void)
{
    /*
     * "Divisible by 4" means the remainder of dividing by 4 is zero, which
     * is what the modulus operator tests: `year % 4 == 0`.
     *
     * The rule in words has two ways to be a leap year:
     *   1. divisible by 4 AND NOT divisible by 100, OR
     *   2. divisible by 400.
     *
     * AND is `&&` and OR is `||`. The parentheses show which parts belong
     * together. (`&&` is evaluated before `||` anyway, but the parentheses
     * make the meaning obvious to a reader, as the lesson recommends.)
     */
    int year = 1900;
    int is_leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    printf("Is %d a leap year? %d\n", year, is_leap); // 0

    // The expression has to be written out again for each year: a variable
    // remembers the RESULT, not the calculation. Lesson 7 shows how to give
    // a calculation a name, so you write it once and use it many times.
    year = 2000;
    is_leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    printf("Is %d a leap year? %d\n", year, is_leap); // 1

    year = 2024;
    is_leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    printf("Is %d a leap year? %d\n", year, is_leap); // 1

    return 0;
}

/*
 * WHY THESE THREE YEARS
 *
 * Each one tests a different part of the rule:
 * - 1900 is divisible by 4 and by 100 but not by 400: not a leap year. An
 *   expression that only checks `year % 4 == 0` gets this one wrong.
 * - 2000 is divisible by 400: a leap year, through the second way.
 * - 2024 is divisible by 4 and not by 100: a leap year, through the first.
 *
 * Add a fourth test of your own, such as 2023, which should give 0.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_leap_year 3_leap_year.c
 *   ./3_leap_year
 */
