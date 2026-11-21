/**
 * @file 1_greet_by_name.c
 * @brief Lesson 1, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Change the message to greet yourself by name, then compile and run it again.
 */

#include <stdio.h>

int main(void)
{
    // Only the text between the double quotes changed. Everything else in
    // the program (the #include, `main`, the braces, `return 0;`) stays the
    // same, because the STRING LITERAL is just data handed to `printf`.
    printf("Hello, Ada!\n");

    return 0;
}

/*
 * The program you run is the EXECUTABLE the compiler made, not the `.c` file.
 * If you change the message and run the old executable, you still see the
 * old message. Compile again after every change:
 *
 *   gcc -Wall -Wextra -std=c17 -o 1_greet_by_name 1_greet_by_name.c
 *   ./1_greet_by_name
 */
