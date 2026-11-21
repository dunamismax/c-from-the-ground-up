/**
 * @file 3_room_to_append.c
 * @brief Lesson 9, Exercise 3: the answer, and a safe version
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Make `full_greeting` smaller, for example `char full_greeting[12]`, and
 * think about what `strcat` would do. Why must the destination be big enough?
 *
 * THE ANSWER:
 * `strcat` would write past the end of the array: a BUFFER OVERFLOW. This
 * program does the arithmetic first and only calls `strcat` when the result
 * fits.
 */

#include <stdio.h>
#include <string.h>

int main(void)
{
    char full_greeting[12] = "Welcome, "; // 9 characters + `\0` = 10 of the 12
    char user[] = "dunamismax";           // 10 characters

    // What the joined string needs: every character of both strings, plus
    // one element for the null terminator.
    size_t needed = strlen(full_greeting) + strlen(user) + 1;

    printf("The array holds 12 characters; the joined string needs %zu.\n", needed);

    if (needed <= 12)
    {
        strcat(full_greeting, user);
        printf("Result of strcat: %s\n", full_greeting);
    }
    else
    {
        // 20 does not fit in 12, so this is the branch that runs.
        printf("Not enough room, so strcat was not called.\n");
        printf("full_greeting is still: \"%s\"\n", full_greeting);
    }

    return 0;
}

/*
 * WHAT WOULD HAVE HAPPENED
 *
 * `strcat` is given two addresses and nothing else. It has no idea the
 * destination is 12 elements long. It finds the `\0` of "Welcome, " and
 * starts copying: `d` and `u` fill the last two elements, and the remaining
 * eight characters and the `\0` are written into memory that does not
 * belong to the array.
 *
 * What that memory was being used for decides what you see. The program may
 * crash, another variable may silently change, or it may appear to work
 * today and fail after an unrelated edit. The language makes no promise
 * (this is called undefined behavior; Lesson 37 is all about it), and
 * attackers have used exactly this mistake to take over programs.
 *
 * So the rule from the lesson: the destination must have room for the whole
 * result AND its `\0`, and since `strcat` and `strcpy` will not check, you
 * must.
 *
 * (`size_t` is the unsigned type `strlen` returns. `%zu` prints it.)
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_room_to_append 3_room_to_append.c
 *   ./3_room_to_append
 */
