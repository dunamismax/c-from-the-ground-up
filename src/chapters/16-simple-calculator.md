# Simple Calculator

Welcome to Part 3! It's time to start building bigger things. This lesson is
our first real PROJECT: a simple calculator that works from the command line.

THE GOAL:
We want to be able to run our program like this:
`./16_simple_calculator 10 + 22`
And have it print the result: `10.00 + 22.00 = 32.00`

SKILLS WE WILL USE:
1. Command-Line Arguments (`argc`, `argv`): To get the numbers and the
   operator from the user when they run the program.
2. Data Conversion: The arguments in `argv` are STRINGS. We need to convert
   the number strings ("10", "22") into actual numbers (`double`) so we can
   do math with them. We'll use the `strtod()` function for this. It is the
   decimal version of the `strtol()` function from Lesson 15, and it tells
   us when an argument is not a number.
3. Conditional Logic (`if`, `switch`): To check if the user provided the
   correct input and to decide which mathematical operation to perform.
4. Error Handling: We will provide a helpful USAGE MESSAGE if the user makes a
   mistake and handle potential math errors like division by zero.

## Full Source

<!-- source: Part 3 - The Advanced Path_ Towards Mastery/16_simple_calculator.c -->
```c
/**
 * @file 16_simple_calculator.c
 * @brief Part 3, Lesson 16: Simple Calculator
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file is our first full PROJECT. It combines several concepts we've
 * learned--command-line arguments, data type conversion, and conditional
 * logic--to build a complete, useful command-line tool.
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
 *    do math with them. We'll use the `strtod()` function for this. It is the
 *    decimal version of the `strtol()` function from Lesson 15, and it tells
 *    us when an argument is not a number.
 * 3. Conditional Logic (`if`, `switch`): To check if the user provided the
 *    correct input and to decide which mathematical operation to perform.
 * 4. Error Handling: We will provide a helpful USAGE MESSAGE if the user makes a
 *    mistake and handle potential math errors like division by zero.
 */

#include <stdio.h>
#include <stdlib.h> // Required for strtod() and exit()
#include <string.h> // Required for strcmp()

// --- Function Prototypes (Lesson 7) ---
// Declaring the function up front gives a summary of the file. Its
// definition, with the explanation, follows below.
int parse_number(const char *text, double *result);

/**
 * @brief Converts a string such as "10.5" to a double.
 * @param text The string to convert.
 * @param result Where to store the number. We pass a pointer so the function
 *               can fill in the caller's variable.
 * @return 1 if the WHOLE string was a number, 0 otherwise.
 *
 * `strtod` ("string to double") works like `strtol` from Lesson 15: it sets
 * `end` to the first character it could not convert. If nothing was
 * converted, or anything is left over (as in "ten" or "5x"), it is not a
 * number. You may also see `atof()` ("ASCII to float") used for this, but
 * `atof("ten")` quietly returns 0.0, so a typo would turn into a wrong answer.
 */
int parse_number(const char *text, double *result)
{
    char *end = NULL;
    *result = strtod(text, &end);
    return end != text && *end == '\0';
}

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

    // `parse_number` (defined above) converts each number string to a double
    // and tells us whether it really was a number.
    if (!parse_number(argv[1], &num1) || !parse_number(argv[3], &num2))
    {
        fprintf(stderr, "Error: '%s' and '%s' must both be numbers.\n", argv[1], argv[3]);
        exit(1);
    }

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
 * - Converted string data to numeric data for calculations (`strtod`), and
 *   rejected arguments that were not numbers.
 * - Implemented robust error checking for both user input and math rules.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c17 -o 16_simple_calculator 16_simple_calculator.c`
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
 *    Not a number (this will trigger our number check):
 *    ./16_simple_calculator ten + 5
 *
 *    Not a single-character operator (this will trigger our operator check):
 *    ./16_simple_calculator 10 plus 5
 *
 *    Too few arguments (this will trigger our usage message):
 *    ./16_simple_calculator 10 +
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c17 -o 16_simple_calculator 16_simple_calculator.c
./16_simple_calculator 5 + 3
```

To multiply, quote the operator, as in `./16_simple_calculator 7 '*' 6`.
Unquoted, the shell expands `*` into the names of the files in the current
directory.

Try the error checks too: `./16_simple_calculator ten + 5` is not a number,
`./16_simple_calculator 10 plus 5` is not a single-character operator,
`./16_simple_calculator 10 / 0` divides by zero, and
`./16_simple_calculator 10 +` prints the usage message.
