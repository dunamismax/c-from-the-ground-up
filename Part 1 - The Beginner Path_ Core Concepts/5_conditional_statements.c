/**
 * @file 5_conditional_statements.c
 * @brief Part 1, Lesson 5: Conditional Statements
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson introduces conditional statements, which allow a program
 * to make decisions and execute different code blocks based on specific conditions.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Our programs so far have been linear; they execute every line of code from
 * top to bottom in the same order every time. To create powerful and intelligent
 * programs, we need them to be able to make decisions.
 *
 * CONDITIONAL STATEMENTS allow us to control the flow of our program's execution.
 * They use the relational and logical operators from the previous lesson to
 * evaluate conditions. If a condition is true (evaluates to a non-zero value),
 * one block of code is executed. If it's false (evaluates to 0), that block
 * is skipped, and optionally, another block is executed.
 *
 * The primary tool for this is the `if` statement.
 */

#include <stdio.h>

int main(void)
{
    // --- Part 1: The `if` Statement ---

    /*
     * The `if` statement is the simplest form of a conditional. It executes a
     * block of code ONLY if its condition is true.
     *
     * Syntax:
     * if (condition) {
     *     // Code to execute if 'condition' is true
     * }
     */
    int score = 100;

    printf("--- Part 1: The 'if' Statement ---\n");
    printf("Initial score: %d\n", score);

    // This condition `score == 100` is true, so the printf inside the braces will run.
    if (score == 100)
    {
        printf("Congratulations on a perfect score!\n");
    }

    // This condition `score > 100` is false, so this block is skipped.
    if (score > 100)
    {
        printf("This message will not be printed.\n");
    }
    printf("\n");

    // --- Part 2: The `if-else` Statement ---

    /*
     * The `else` statement provides an alternative block of code to execute
     * when the `if` condition is false.
     *
     * Syntax:
     * if (condition) {
     *     // Code to execute if 'condition' is true
     * } else {
     *     // Code to execute if 'condition' is false
     * }
     */
    int number_to_check;

    printf("--- Part 2: The 'if-else' Statement ---\n");
    printf("Enter a whole number: ");
    scanf("%d", &number_to_check);

    // We use the modulus operator `%` to check for an even number.
    // If a number divided by 2 has a remainder of 0, it's even.
    if (number_to_check % 2 == 0)
    {
        printf("%d is an even number.\n", number_to_check);
    }
    else
    {
        printf("%d is an odd number.\n", number_to_check);
    }
    printf("\n");

    // --- Part 3: The `if-else if-else` Chain ---

    /*
     * To check for multiple, mutually exclusive conditions, you can chain
     * `else if` statements. The program will check each condition in order.
     * As soon as it finds a true condition, it runs that block and skips the
     * rest of the entire chain. The final `else` is optional and acts as a
     * "catch-all" if none of the preceding conditions were true.
     */
    int user_grade;

    printf("--- Part 3: The 'if-else if-else' Chain ---\n");
    printf("Enter your numerical grade (0-100): ");
    scanf("%d", &user_grade);

    if (user_grade >= 90)
    {
        printf("Your letter grade is A.\n");
    }
    else if (user_grade >= 80)
    {
        printf("Your letter grade is B.\n");
    }
    else if (user_grade >= 70)
    {
        printf("Your letter grade is C.\n");
    }
    else if (user_grade >= 60)
    {
        printf("Your letter grade is D.\n");
    }
    else
    {
        printf("Your letter grade is F.\n");
    }

    /*
     * Note on logical combinations:
     * We can use logical operators (`&&`, `||`) to create more complex conditions.
     * For example, to check if a grade is a B (80-89):
     */
    if (user_grade >= 80 && user_grade < 90)
    {
        printf("This is a solid B.\n");
    }

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you've learned how to give your program a brain.
 *
 * Key Takeaways:
 * - The `if` statement executes code only if a condition is true.
 * - The `if-else` statement provides an alternative path for when the condition is false.
 * - The `if-else if-else` chain allows you to test a series of conditions in order,
 *   executing only the first one that is true.
 * - These constructs are the fundamental building blocks for creating logic
 *   and making your programs smart and responsive.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 5_conditional_statements 5_conditional_statements.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./5_conditional_statements`
 *    - On Windows:       `5_conditional_statements.exe`
 */