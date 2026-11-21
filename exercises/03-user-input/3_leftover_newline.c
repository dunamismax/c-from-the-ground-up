/**
 * @file 3_leftover_newline.c
 * @brief Lesson 3, Exercise 3: what happens, and why
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Remove the space in `" %c"` and run the program again. What happens at the
 * letter prompt?
 *
 * THE ANSWER:
 * The program does not wait for you. It prints the letter prompt and at once
 * reports a "letter" that is really the Enter key you pressed after the GPA.
 * This file is that broken program, so you can watch it happen.
 */

#include <stdio.h>

int main(void)
{
    double user_gpa = 0.0;
    printf("Please enter your GPA (e.g., 3.8): ");
    if (scanf("%lf", &user_gpa) != 1)
    {
        printf("Could not read a number. Exiting.\n");
        return 1;
    }
    printf("Your GPA is %.2f.\n\n", user_gpa);

    char favorite_letter = '?';
    printf("Please enter your favorite letter: ");

    // THE BUG: no space before %c. Compare with the lesson's `" %c"`.
    if (scanf("%c", &favorite_letter) != 1)
    {
        printf("Could not read a letter. Exiting.\n");
        return 1;
    }

    printf("Your favorite letter is '%c'.\n", favorite_letter);

    // Printing the character's code (Lesson 2, Exercise 3) shows what was
    // read: 10 is the code of the newline character, `\n`.
    printf("Its character code is %d.\n", favorite_letter);

    return 0;
}

/*
 * WHY IT HAPPENS
 *
 * When you type `3.8` and press Enter, four characters go into the INPUT
 * BUFFER: `3`, `.`, `8` and `\n`. `%lf` takes the first three and stops at
 * the newline, leaving it behind.
 *
 * `%d` and `%lf` skip whitespace before they start reading, so a leftover
 * newline never bothers them. `%c` is different: it reads the very next
 * character, whatever it is. Here that is the leftover `\n`, so `scanf`
 * returns straight away, with success, and the output looks like this:
 *
 *   Your favorite letter is '
 *   '.
 *   Its character code is 10.
 *
 * The space in `" %c"` means "first skip any whitespace". With it, the
 * newline is thrown away and `scanf` waits for a real character.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_leftover_newline 3_leftover_newline.c
 *   ./3_leftover_newline
 */
