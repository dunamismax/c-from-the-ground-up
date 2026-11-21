/**
 * @file 1_age_this_year.c
 * @brief Lesson 3, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Ask for the user's birth year and print how old they turn this year.
 */

#include <stdio.h>

int main(void)
{
    // The program cannot know what year it is yet (reading the computer's
    // clock comes much later), so it asks for that too.
    int current_year = 0;
    int birth_year = 0;

    printf("What year is it now? ");
    if (scanf("%d", &current_year) != 1)
    {
        printf("Could not read a year. Exiting.\n");
        return 1;
    }

    printf("What year were you born? ");
    if (scanf("%d", &birth_year) != 1)
    {
        printf("Could not read a year. Exiting.\n");
        return 1;
    }

    // `-` subtracts one number from the other. Lesson 4 covers it and the
    // other arithmetic operators.
    int age = current_year - birth_year;
    printf("This year you turn %d.\n", age);

    return 0;
}

/*
 * Each `scanf` call is checked on its own, with the pattern from the lesson.
 * If the first one fails, the program stops before it asks the second
 * question, so it never calculates with a value the user did not give.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_age_this_year 1_age_this_year.c
 *   ./1_age_this_year
 */
