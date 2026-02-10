/**
 * @file 6_loops.c
 * @brief Part 1, Lesson 6: Loops
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson introduces loops, which are control structures that allow
 * a block of code to be executed repeatedly.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Imagine you needed to print "Hello!" five times. You could write:
 * printf("Hello!\n");
 * printf("Hello!\n");
 * printf("Hello!\n");
 * printf("Hello!\n");
 * printf("Hello!\n");
 *
 * This is tedious and not scalable. What if you needed to do it 500 times?
 * This is where LOOPS come in. A LOOP is a programming structure that repeats a
 * sequence of instructions until a specific condition is met. Each single
 * execution of the loop's body is called an ITERATION.
 *
 * C has three main types of loops:
 * 1. The `for` loop
 * 2. The `while` loop
 * 3. The `do-while` loop
 */

#include <stdio.h>

int main(void)
{
    // --- Part 1: The `for` Loop ---

    /*
     * The `for` loop is ideal when you know exactly how many times you want to
     * repeat a block of code. It has a very specific structure with three parts
     * inside its parentheses, separated by semicolons.
     *
     * for (INITIALIZATION; CONDITION; POST-ITERATION) {
     *     // Code to be repeated
     * }
     *
     * 1. INITIALIZATION: A statement that runs only ONCE before the loop starts.
     *    It's typically used to declare and initialize a counter variable.
     * 2. CONDITION: An expression that is checked BEFORE each iteration. If the
     *    condition is true, the loop body runs. If it's false, the loop terminates.
     * 3. POST-ITERATION: A statement that runs AFTER each iteration. It's
     *    typically used to increment or decrement the counter variable. `i++` is
     *    shorthand for `i = i + 1`.
     */
    printf("--- Part 1: The 'for' Loop (Countdown) ---\n");

    // Let's create a countdown from 5 down to 1.
    // Initialization: `int i = 5`
    // Condition: `i > 0` (loop as long as i is greater than 0)
    // Post-iteration: `i--` (decrement i by 1 after each loop; shorthand for i = i - 1)
    for (int i = 5; i > 0; i--)
    {
        printf("%d...\n", i);
    }
    printf("Blast off!\n\n");

    // --- Part 2: The `while` Loop ---

    /*
     * The `while` loop is simpler. It repeats a block of code AS LONG AS its
     * condition remains true. It's useful when you don't know the exact number
     * of iterations beforehand.
     *
     * while (condition) {
     *     // Code to be repeated
     * }
     *
     * The condition is checked BEFORE each iteration.
     *
     * CRITICAL: You must ensure that something inside the loop's body will
     * eventually make the condition false. Otherwise, you will create an
     * INFINITE LOOP, and your program will never end!
     */
    printf("--- Part 2: The 'while' Loop (Simple Menu) ---\n");

    int menu_choice = 0;

    // Loop as long as the user has not chosen option 4 to quit.
    while (menu_choice != 4)
    {
        printf("Menu:\n");
        printf("1. Start New Game\n");
        printf("2. Load Game\n");
        printf("3. Options\n");
        printf("4. Quit\n");
        printf("Enter your choice: ");

        scanf("%d", &menu_choice); // Get user input. This updates the loop variable.

        printf("You chose: %d\n\n", menu_choice);
    }
    printf("Thanks for playing!\n\n");

    // --- Part 3: The `do-while` Loop ---

    /*
     * The `do-while` loop is a variation of the `while` loop. The key difference
     * is that the condition is checked AT THE END of the loop's body.
     *
     * This means a `do-while` loop is GUARANTEED to execute at least once.
     *
     * This makes it perfect for situations like input validation, where you need
     * to get the input first *before* you can check if it's valid.
     */
    printf("--- Part 3: The 'do-while' Loop (Input Validation) ---\n");
    int secret_number;

    do
    {
        printf("Please enter a number between 1 and 10: ");
        scanf("%d", &secret_number);

        // This check happens AFTER the user has already entered a number.
        if (secret_number < 1 || secret_number > 10)
        {
            printf("Invalid input. Please try again.\n");
        }

    } while (secret_number < 1 || secret_number > 10); // The condition is checked here.

    printf("You successfully entered the number %d.\n", secret_number);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you learned how to make your program perform repetitive tasks.
 *
 * Key Takeaways:
 * - Loops are used to execute a block of code multiple times.
 * - The `for` loop is great for when you know the number of iterations in advance.
 *   It consists of an initializer, a condition, and a post-iteration step.
 * - The `while` loop is great for when you want to loop as long as a condition is
 *   true, and the number of iterations isn't fixed.
 * - The `do-while` loop is similar to `while`, but it always executes its body at
 *   least once because the condition is checked at the end.
 *
 * Mastering loops is a massive step towards writing complex and useful programs.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 6_loops 6_loops.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./6_loops`
 *    - On Windows:       `6_loops.exe`
 */