/**
 * @file 3_average_temperature.c
 * @brief Lesson 8, Exercise 3: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Compute the average of `temperatures` as a `double` and print it with one
 * decimal place.
 */

#include <stdio.h>

int main(void)
{
    double temperatures[] = {72.5, 75.0, 69.8, 80.1, 85.3}; // Size is 5

    // Add the elements up, exactly as the lesson sums `high_scores`. The
    // total is a `double` because the elements are.
    double total = 0.0;
    for (int i = 0; i < 5; i++)
    {
        total = total + temperatures[i];
    }

    // The average is the total divided by how many values there are.
    // `total` is a `double`, so this is floating-point division and the
    // fractional part is kept.
    double average = total / 5;

    printf("Total: %.1f\n", total);                 // 382.7
    printf("Average temperature: %.1f\n", average); // 76.5

    return 0;
}

/*
 * WATCH OUT WITH INTEGER ARRAYS
 *
 * Here the division is safe because `total` is a `double`. Do the same with
 * an `int` total, such as the lesson's `total_score`, and `total_score / 5`
 * is INTEGER DIVISION (Lesson 4): the fraction is thrown away before the
 * result is ever stored. For an average of integers, cast first:
 * `(double)total_score / 5`.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_average_temperature 3_average_temperature.c
 *   ./3_average_temperature
 */
