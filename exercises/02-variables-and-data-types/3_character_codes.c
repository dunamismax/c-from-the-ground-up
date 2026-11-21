/**
 * @file 3_character_codes.c
 * @brief Lesson 2, Exercise 3: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Print a `char` variable with `%d` instead of `%c`. The number you see is
 * the character's code.
 */

#include <stdio.h>

int main(void)
{
    char player_grade = 'A';
    char digit = '7';

    // The same variable, printed two ways. `%c` shows the character and
    // `%d` shows the number that is really stored in the variable.
    printf("As a character: %c\n", player_grade);
    printf("As a number:    %d\n", player_grade); // 65

    // A digit character is not the number it looks like: '7' is stored as
    // the code 55, not as 7.
    printf("The character '%c' has the code %d.\n", digit, digit);

    return 0;
}

/*
 * WHAT IS GOING ON
 *
 * A computer stores only numbers. A `char` is a small integer, and each
 * character is given a number by a table called ASCII: 'A' is 65, 'B' is 66,
 * 'a' is 97, '0' is 48 and so on. The format specifier decides how `printf`
 * shows that number: `%c` draws the character with that code, `%d` prints
 * the code itself.
 *
 * This is why `'7'` and `7` are different things in C: the first is the
 * character with code 55, the second is the integer seven.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_character_codes 3_character_codes.c
 *   ./3_character_codes
 */
