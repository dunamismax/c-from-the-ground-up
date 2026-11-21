/**
 * @file 2_grade_range.c
 * @brief Lesson 5, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Reject grades outside 0-100 with an error message before the grade chain
 * runs.
 */

#include <stdio.h>

int main(void)
{
    int user_grade = 0;

    printf("Enter your numerical grade (0-100): ");
    if (scanf("%d", &user_grade) != 1)
    {
        printf("That was not a whole number. Exiting.\n");
        return 1;
    }

    // Reading a number is not the same as reading a SENSIBLE number. The
    // lesson's chain would call 250 an A and -40 an F. A grade is wrong if
    // it is below 0 OR above 100, so the two tests are joined with `||`.
    //
    // Checking first and returning early keeps the chain below simple: by
    // the time it runs, the grade is known to be in range.
    if (user_grade < 0 || user_grade > 100)
    {
        printf("Error: %d is not a grade between 0 and 100.\n", user_grade);
        return 1;
    }

    if (user_grade >= 90)
    {
        printf("Your letter grade is A.\n");
    }
    else if (user_grade >= 80)
    {
        printf("Your letter grade is B.\n");
    }
    else if (user_grade >= 70)
    {
        printf("Your letter grade is C.\n");
    }
    else if (user_grade >= 60)
    {
        printf("Your letter grade is D.\n");
    }
    else
    {
        printf("Your letter grade is F.\n");
    }

    return 0;
}

/*
 * Try -1, 0, 100 and 101. The first and last must be rejected; 0 and 100
 * are real grades and must not be. `<` and `>` (rather than `<=` and `>=`)
 * are what let 0 and 100 through.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_grade_range 2_grade_range.c
 *   ./2_grade_range
 */
