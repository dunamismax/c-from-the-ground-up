/**
 * @file 2_variables_and_data_types.c
 * @brief Part 1, Lesson 2: Variables and Data Types
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file introduces the fundamental concepts of variables and data types.
 * We learn how to store information in memory and print it to the console.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * In the last lesson, we printed a fixed message. To write useful programs,
 * we need a way to store and manipulate data that can change. This is where
 * variables come in.
 *
 * WHAT IS A VARIABLE?
 * A VARIABLE is a named storage location in the computer's memory. Think of it
 * as a labeled box where you can store a piece of information. You give it a
 * name, and you can change the contents of the box (the value) as your
 * program runs.
 *
 * WHAT IS A DATA TYPE?
 * A DATA TYPE specifies the type of data that a variable can hold. C is a
 * "statically-typed" language, which means you must explicitly tell the
 * compiler the data type of every variable you create. This helps the compiler
 * allocate the correct amount of memory and ensures you use the variable correctly.
 *
 * The process of creating a variable is called DECLARATION.
 * The process of giving a variable its first value is called INITIALIZATION.
 */

#include <stdio.h>

int main(void)
{
    // The main function body is where we'll declare and use our variables.

    // --- Part 1: The Integer (int) ---

    /*
     * An INTEGER is a whole number (no decimal point). The `int` data type is
     * used to store them.
     *
     * Below, we DECLARE a variable named `player_score` of type `int`.
     * This tells the compiler: "Reserve a spot in memory for an integer,
     * and I will call it `player_score`."
     */
    int player_score; // Declaration

    /*
     * Now we INITIALIZE the variable by assigning it a value using the
     * assignment operator `=`.
     */
    player_score = 1250; // Initialization

    /*
     * To print the value of a variable, we use `printf` with a FORMAT SPECIFIER.
     * A FORMAT SPECIFIER is a placeholder in the string that tells `printf`
     * what type of data to expect and how to format it.
     *
     * `%d` is the format specifier for a signed decimal integer (`int`).
     * The variable `player_score` is passed as a second argument to `printf`,
     * and its value will replace `%d` in the output.
     */
    printf("Player score: %d\n", player_score);

    // You can also declare and initialize a variable in a single statement.
    int number_of_lives = 3;
    printf("Number of lives: %d\n", number_of_lives);
    printf("\n"); // Print a blank line for separation

    // --- Part 2: The Double-Precision Floating-Point Number (double) ---

    /*
     * To store numbers with decimal points, we use FLOATING-POINT types.
     * The most common one is `double`, which stands for "double-precision".
     * It can store numbers with much greater precision than its counterpart, `float`.
     * For this course, we will prefer `double` for all decimal numbers.
     *
     * `%f` is the format specifier for a `double` (and `float`).
     */
    double item_price = 19.99; // Declaration and initialization
    printf("The price of the magic sword is: $%f\n", item_price);

    /*
     * By default, `%f` might print many extra zeros. You can control the
     * number of decimal places by modifying the format specifier. For example,
     * `%.2f` tells `printf` to print only 2 digits after the decimal point.
     */
    printf("The price, formatted neatly, is: $%.2f\n", item_price);
    printf("\n");

    // --- Part 3: The Character (char) ---

    /*
     * The `char` data type is used to store a single CHARACTER. This can be
     * a letter, a number, or a symbol.
     *
     * A CHARACTER LITERAL is enclosed in SINGLE quotes (e.g., 'A'). This is
     * different from a string literal, which uses double quotes.
     *
     * `%c` is the format specifier for a `char`.
     */
    char player_grade = 'A'; // Declaration and initialization
    printf("Player's performance grade: %c\n", player_grade);

    // --- Part 4: Putting It All Together ---

    /*
     * You can print multiple variables in a single `printf` call. Just make
     * sure the format specifiers and the variables are listed in the same order.
     */
    printf("Summary: Player with %d lives and a grade of '%c' has a score of %d.\n",
           number_of_lives, player_grade, player_score);

    return 0; // Signal successful execution
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you learned about three fundamental data types:
 * - `int` for whole numbers.
 * - `double` for numbers with decimal points.
 * - `char` for single characters.
 *
 * You also learned how to DECLARE variables, INITIALIZE them with values, and
 * print them using `printf` with FORMAT SPECIFIERS (`%d`, `%f`, `%c`).
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 2_variables_and_data_types 2_variables_and_data_types.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./2_variables_and_data_types`
 *    - On Windows:       `2_variables_and_data_types.exe`
 */