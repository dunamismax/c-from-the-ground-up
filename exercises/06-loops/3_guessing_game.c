/**
 * @file 3_guessing_game.c
 * @brief Lesson 6, Exercise 3: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Write a number-guessing game: keep asking until the user types 7, and count
 * the guesses. Make sure garbage input and Ctrl+D do not make it loop
 * forever.
 */

#include <stdio.h>

int main(void)
{
    int guess = 0;
    int guess_count = 0;

    // The player must guess at least once, so the question comes before the
    // test. That is what `do-while` is for.
    do
    {
        printf("Guess the number: ");
        int items_read = scanf("%d", &guess);

        if (items_read == EOF)
        {
            // Ctrl+D: no more guesses can ever arrive. Without this check
            // every later `scanf` would fail the same way and the loop
            // would spin forever.
            printf("\nNo more input. The number was 7.\n");
            return 1;
        }

        if (items_read == 0)
        {
            // Garbage such as "seven". Throw the rest of the line away so
            // the next `scanf` starts on fresh input, and do not count it
            // as a guess.
            int leftover = getchar();
            while (leftover != '\n' && leftover != EOF)
            {
                leftover = getchar();
            }
            printf("Please type a whole number.\n");
            guess = 0; // Anything but 7, so the loop asks again.
        }
        else
        {
            guess_count++;
            if (guess != 7)
            {
                printf("No, try again.\n");
            }
        }

    } while (guess != 7);

    printf("Correct! You needed %d guesses.\n", guess_count);

    return 0;
}

/*
 * THE TWO WAYS THIS LOOP COULD HANG
 *
 * 1. Bad text stays in the input buffer. `scanf("%d")` stops at the first
 *    character that cannot be part of a number and leaves it there. Ask
 *    again without removing it and `scanf` fails on the same character,
 *    again and again. The `getchar` loop removes it.
 * 2. Input ends. At end of input every read fails at once, so a loop that
 *    only says "try again" never stops. The `EOF` check ends the program.
 *
 * Test both: type `seven`, then press Ctrl+D on a later run.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_guessing_game 3_guessing_game.c
 *   ./3_guessing_game
 */
