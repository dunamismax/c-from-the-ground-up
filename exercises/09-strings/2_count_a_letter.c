/**
 * @file 2_count_a_letter.c
 * @brief Lesson 9, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Count how many times the letter `s` appears in `message`.
 */

#include <stdio.h>

int main(void)
{
    char message[] = "This is a test.";
    int count = 0;

    // Visit every character up to (not including) the null terminator.
    for (int i = 0; message[i] != '\0'; i++)
    {
        // A single character is compared with `==` and SINGLE quotes.
        // (`strcmp` is for whole strings; "s" in double quotes would be a
        // string, not a character.)
        if (message[i] == 's')
        {
            count++;
        }
    }

    printf("The message is: \"%s\"\n", message);
    printf("The letter 's' appears %d times.\n", count); // 3

    return 0;
}

/*
 * The three are in "This", "is" and "test". The comparison is exact, so a
 * capital 'S' would not be counted: 's' and 'S' are different characters
 * with different codes (115 and 83). To count both, test for either one:
 *
 *   if (message[i] == 's' || message[i] == 'S')
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_count_a_letter 2_count_a_letter.c
 *   ./2_count_a_letter
 */
