/**
 * @file 1_count_characters.c
 * @brief Lesson 9, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Write a loop that counts the characters in a string without calling
 * `strlen`, by stopping at `'\0'`.
 */

#include <stdio.h>
#include <string.h> // Only for strlen(), to check our answer.

int main(void)
{
    char message[] = "This is a test.";

    // Walk along the array until the element is the null terminator. When
    // the loop ends, `length` is the index of the `\0`, which is also the
    // number of characters in front of it.
    int length = 0;
    while (message[length] != '\0')
    {
        length++;
    }

    printf("The message is: \"%s\"\n", message);
    printf("Counted by hand: %d characters\n", length);
    printf("According to strlen: %zu characters\n", strlen(message));

    // An empty string is just a `\0`. The loop condition is false at once,
    // so the count is 0.
    char empty[] = "";
    length = 0;
    while (empty[length] != '\0')
    {
        length++;
    }
    printf("An empty string has %d characters.\n", length);

    return 0;
}

/*
 * This loop is, in essence, what `strlen` does inside. It shows why the null
 * terminator matters so much: nothing else tells the loop where the string
 * stops. If the `\0` were missing, the loop would keep walking past the end
 * of the array into whatever memory comes next.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_count_characters 1_count_characters.c
 *   ./1_count_characters
 */
