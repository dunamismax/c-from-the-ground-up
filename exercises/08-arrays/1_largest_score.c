/**
 * @file 1_largest_score.c
 * @brief Lesson 8, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Find and print the largest value in `high_scores`.
 */

#include <stdio.h>

int main(void)
{
    // The lesson's scores happen to be sorted, which would hide a wrong
    // answer, so this solution uses the same five values in a mixed order.
    int high_scores[5] = {875, 980, 799, 950, 820};

    // "Best so far", as in Lesson 5, Exercise 3: assume the first element
    // is the largest, then let every other element challenge it.
    int largest = high_scores[0];

    // The loop can start at index 1, because index 0 is already in
    // `largest`. It stops before 5: the valid indices are 0 to 4.
    for (int i = 1; i < 5; i++)
    {
        if (high_scores[i] > largest)
        {
            largest = high_scores[i];
        }
    }

    printf("The largest score is: %d\n", largest); // 980

    return 0;
}

/*
 * WHY NOT START WITH `int largest = 0;`?
 *
 * It works for these scores, but it gives the wrong answer for an array of
 * negative numbers such as {-5, -2, -9}: nothing is greater than 0, so the
 * program would print 0, a value that is not in the array at all. Starting
 * from an element of the array is always right.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_largest_score 1_largest_score.c
 *   ./1_largest_score
 */
