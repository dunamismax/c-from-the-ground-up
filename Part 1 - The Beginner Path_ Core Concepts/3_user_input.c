/**
 * @file 3_user_input.c
 * @brief Part 1, Lesson 3: User Input
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson demonstrates how to make programs interactive by reading
 * input from the user's keyboard.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, our programs have been one-way streets: they run and print things
 * without any input from the outside world. To build truly useful tools,
 * we need our programs to be interactive.
 *
 * THE `scanf` FUNCTION
 * The counterpart to `printf` (for printing) is `scanf` (for scanning/reading).
 * The `scanf` function reads formatted input from the standard input, which is
 * usually your keyboard.
 *
 * Like `printf`, it uses FORMAT SPECIFIERS to know what type of data to expect.
 * However, it has one very important new requirement we will explore.
 */

#include <stdio.h>

int main(void)
{
    // --- Part 1: Reading an Integer ---

    /*
     * To read input, we first need a variable to store that input in.
     * We DECLARE an integer variable `user_age`. Notice we do not initialize it,
     * because its value will be provided by the user.
     */
    int user_age;

    /*
     * It's good practice to print a "prompt" to the user, so they know
     * what you're asking for.
     */
    printf("Please enter your age (as a whole number): ");

    /*
     * Now, we call `scanf`. This is the most important part of the lesson.
     *
     * - `"%d"`: This is the format specifier. It tells `scanf` to look for
     *   and read an integer from the keyboard input.
     *
     * - `&user_age`: This is new! The `&` symbol is the ADDRESS-OF OPERATOR.
     *   It gets the memory address of the `user_age` variable.
     *
     * WHY DO WE NEED `&`?
     * You must tell `scanf` *where* in memory to store the data it reads.
     * Simply providing `user_age` would only give `scanf` the variable's
     * *current value* (which is garbage at this point). By providing `&user_age`,
     * you are giving it the *location* of the box, so it can put the user's
     * input directly into it.
     */
    scanf("%d", &user_age);

    // Let's print the value to confirm we received it correctly.
    printf("Thank you. You entered %d.\n", user_age);
    printf("\n"); // Add a blank line for spacing.

    // --- Part 2: Reading a Double ---

    /*
     * The pattern is the same for other data types. We just need to change
     * the variable type and the format specifier.
     *
     * IMPORTANT: For `scanf`, the format specifier for a `double` is `%lf`
     * (which stands for "long float"). This is DIFFERENT from `printf`,
     * which uses `%f` for a `double`. This is a common point of confusion!
     */
    double user_gpa;
    printf("Please enter your GPA (e.g., 3.8): ");
    scanf("%lf", &user_gpa); // Use %lf for reading a double!

    printf("Your GPA is %.2f.\n", user_gpa); // Use %f for printing a double.
    printf("\n");

    // --- Part 3: A Common `scanf` Pitfall and Reading a Character ---

    /*
     * `scanf` can sometimes behave in unexpected ways because of how it handles
     * whitespace (like spaces, tabs, and newlines).
     *
     * When you typed your GPA and pressed Enter, the `\n` (newline character)
     * was left behind in the INPUT BUFFER. A subsequent `scanf` call for a
     * character (`%c`) would read that leftover newline instead of waiting for
     * you to type a new character!
     *
     * THE FIX: To fix this, we put a single space before the `%c` in the format
     * string: `" %c"`. That leading space tells `scanf` to "skip any and all
     * leading whitespace characters before reading the character". It's a crucial trick.
     */
    char favorite_letter;
    printf("Please enter your favorite letter: ");
    scanf(" %c", &favorite_letter); // The space before %c is very important!

    printf("Your favorite letter is '%c'.\n", favorite_letter);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you learned the basics of making your programs interactive.
 *
 * Key Takeaways:
 * - The `scanf()` function reads formatted input from the keyboard.
 * - `scanf()` requires the ADDRESS-OF OPERATOR `&` before the variable name to know
 *   *where* to store the input.
 * - Format specifiers for `scanf()` can sometimes be different from `printf()`.
 *   - For `double`: Use `%lf` with `scanf`, but `%f` with `printf`.
 * - `scanf()` can leave newline characters in the input buffer, which can cause
 *   problems. The `" %c"` trick (with a leading space) is used to solve this
 *    when reading characters.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 3_user_input 3_user_input.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./3_user_input`
 *    - On Windows:       `3_user_input.exe`
 */