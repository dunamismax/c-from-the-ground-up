/**
 * @file 16_simple_calculator.c
 * @brief Part 3, Lesson 16: Simple Calculator
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file is our first full PROJECT. It combines several concepts we've
 * learned—command-line arguments, data type conversion, and conditional
 * logic—to build a complete, useful command-line tool.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Welcome to Part 3! It's time to start building bigger things. This lesson is
 * our first real PROJECT: a simple calculator that works from the command line.
 *
 * THE GOAL:
 * We want to be able to run our program like this:
 * `./16_simple_calculator 10 + 22`
 * And have it print the result: `10.00 + 22.00 = 32.00`
 *
 * SKILLS WE WILL USE:
 * 1. Command-Line Arguments (`argc`, `argv`): To get the numbers and the
 *    operator from the user when they run the program.
 * 2. Data Conversion: The arguments in `argv` are STRINGS. We need to convert
 *    the number strings ("10", "22") into actual numbers (`double`) so we can
 *    do math with them. We'll use the `atof()` function for this.
 * 3. Conditional Logic (`if`, `switch`): To check if the user provided the
 *    correct input and to decide which mathematical operation to perform.
 * 4. Error Handling: We will provide a helpful USAGE MESSAGE if the user makes a
 *    mistake and handle potential math errors like division by zero.
 */

#include <stdio.h>
#include <stdlib.h> // Required for atof() and exit()
#include <string.h> // Required for strcmp()

// We need argc and argv to get input from the command line.
int main(int argc, char *argv[])
{
    double num1, num2, result;
    char operator;

    // --- Step 1: Validate the Input ---
    // We need exactly four arguments:
    // argv[0]: program name
    // argv[1]: first number
    // argv[2]: operator
    // argv[3]: second number
    if (argc != 4)
    {
        // If the count is wrong, print a helpful usage message to standard error.
        fprintf(stderr, "Usage: %s <number1> <operator> <number2>\n", argv[0]);
        fprintf(stderr, "Example: %s 10.5 / 2\n", argv[0]);
        exit(1); // Exit with an error code.
    }

    // --- Step 2: Convert and Store Arguments ---

    // `atof` (ASCII to float) converts a string to a double.
    num1 = atof(argv[1]);
    num2 = atof(argv[3]);

    // The operator is a string like "+". We only need the first character.
    // We check if the string is longer than one character, which would be invalid.
    if (strlen(argv[2]) != 1)
    {
        fprintf(stderr, "Error: Invalid operator '%s'. Operator must be a single character.\n", argv[2]);
        exit(1);
    }
    operator = argv[2][0];

    // --- Step 3: Perform the Calculation ---
    // A `switch` statement is perfect for checking the operator and deciding
    // which calculation to perform.
    switch (operator)
    {
    case '+':
        result = num1 + num2;
        break; // `break` exits the switch statement.

    case '-':
        result = num1 - num2;
        break;

    case '*':
        // Some shells interpret * as a wildcard. Users may need to type '*'
        // to pass it correctly.
        result = num1 * num2;
        break;

    case '/':
        // This is an important EDGE CASE. We cannot divide by zero.
        if (num2 == 0)
        {
            fprintf(stderr, "Error: Division by zero is not allowed.\n");
            exit(1);
        }
        result = num1 / num2;
        break;

    default:
        // If the operator is not one of the above, it's invalid.
        fprintf(stderr, "Error: Unknown operator '%c'.\n", operator);
        fprintf(stderr, "Supported operators are: + - * /\n");
        exit(1);
    }

    // --- Step 4: Display the Result ---
    // If we get here, the calculation was successful.
    // We use %.2f to format the numbers to two decimal places for clean output.
    printf("%.2f %c %.2f = %.2f\n", num1, operator, num2, result);

    return 0; // Success!
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Congratulations! You've just built your first complete, functional command-line
 * tool. You've seen how to take user input, validate it, process it, and
 * produce a result, all while handling potential errors gracefully.
 *
 * Key Achievements in this Project:
 * - Combined multiple C features into a working application.
 * - Used `argc` and `argv` for real-world program input.
 * - Converted string data to numeric data for calculations (`atof`).
 * - Implemented robust error checking for both user input and math rules.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 16_simple_calculator 16_simple_calculator.c`
 *
 * 4. Run the executable with different arguments:
 *
 *    Addition:
 *    ./16_simple_calculator 5 + 3
 *
 *    Subtraction with decimals:
 *    ./16_simple_calculator 100.5 - 50.2
 *
 *    Multiplication (you might need quotes around '*' in some terminals like bash):
 *    ./16_simple_calculator 7 '*' 6
 *
 *    Division:
 *    ./16_simple_calculator 20 / 4
 *
 *    Division by zero (this will trigger our error message):
 *    ./16_simple_calculator 10 / 0
 *
 *    Incorrect usage (this will trigger our usage message):
 *    ./16_simple_calculator 10 plus 5
 */