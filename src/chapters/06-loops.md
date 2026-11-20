# Loops

Imagine you needed to print "Hello!" five times. You could write:
printf("Hello!\n");
printf("Hello!\n");
printf("Hello!\n");
printf("Hello!\n");
printf("Hello!\n");

This is tedious and not scalable. What if you needed to do it 500 times?
This is where LOOPS come in. A LOOP is a programming structure that repeats a
sequence of instructions until a specific condition is met. Each single
execution of the loop's body is called an ITERATION.

C has three main types of loops:
1. The `for` loop
2. The `while` loop
3. The `do-while` loop

The `for` loop is ideal when you know exactly how many times you want to
repeat a block of code. It has a very specific structure with three parts
inside its parentheses, separated by semicolons.

```c
for (INITIALIZATION; CONDITION; POST-ITERATION) {
    // Code to be repeated
}
```

1. INITIALIZATION: A statement that runs only ONCE before the loop starts.
   It's typically used to declare and initialize a counter variable.
2. CONDITION: An expression that is checked BEFORE each iteration. If the
   condition is true, the loop body runs. If it's false, the loop terminates.
3. POST-ITERATION: A statement that runs AFTER each iteration. It's
   typically used to increment or decrement the counter variable. `i++` is
   shorthand for `i = i + 1`.

The `while` loop is simpler. It repeats a block of code AS LONG AS its
condition remains true. It's useful when you don't know the exact number
of iterations beforehand.

```c
while (condition) {
    // Code to be repeated
}
```

The condition is checked BEFORE each iteration.

CRITICAL: You must ensure that something inside the loop's body will
eventually make the condition false. Otherwise, you will create an
INFINITE LOOP, and your program will never end!

The `do-while` loop is a variation of the `while` loop. The key difference
is that the condition is checked AT THE END of the loop's body.

This means a `do-while` loop is GUARANTEED to execute at least once.

This makes it perfect for situations like input validation, where you need
to get the input first *before* you can check if it's valid.

## Reading Input Inside a Loop

Loops and `scanf` together are a classic source of infinite loops, so this
lesson handles every result `scanf` can return:

- `1`: the number was read. Use it.
- `EOF`: the input has ended (for example, Ctrl+D), so no more input will
  ever arrive. Leave the loop instead of asking forever.
- `0`: the user typed something that is not a number, like `abc`. `scanf`
  leaves that text waiting in the INPUT BUFFER, so the next call would fail on
  the same text again. Throw the rest of the line away first:

```c
int leftover = getchar();
while (leftover != '\n' && leftover != EOF)
{
    leftover = getchar();
}
```

`getchar()` reads ONE character and returns it as an `int`. It returns an
`int` rather than a `char` so that it can also return `EOF`.

## Full Source

<!-- source: Part 1 - The Beginner Path_ Core Concepts/6_loops.c -->
```c
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

        /*
         * Reading input inside a loop needs extra care. As you saw in Lesson 3,
         * `scanf` returns how many items it read. We save that in a variable so
         * we can handle each of its three possible outcomes.
         */
        int items_read = scanf("%d", &menu_choice); // This updates the loop variable.

        if (items_read == 1)
        {
            printf("You chose: %d\n\n", menu_choice);
        }
        else if (items_read == EOF)
        {
            /*
             * The input has ended (for example, the user pressed Ctrl+D), so no
             * more choices will ever arrive. If we did nothing, `menu_choice`
             * would never become 4 and this would be an INFINITE LOOP. Setting
             * it to 4 makes the loop condition false, so the loop ends.
             */
            printf("\nNo more input. Quitting.\n");
            menu_choice = 4;
        }
        else
        {
            /*
             * The user typed something that is not a number, like `abc`.
             * `scanf` refuses to read it and leaves it waiting in the INPUT
             * BUFFER. If we just looped back, the next `scanf` would see the
             * same `abc`, fail again, and loop forever!
             *
             * To fix this, we throw away everything up to the end of the line.
             * `getchar()` reads ONE character from the input and returns it. We
             * keep reading until we reach the newline `'\n'` (the user pressed
             * Enter) or `EOF` (the input ended). `getchar` returns an `int`, not
             * a `char`, so that it can also return `EOF`.
             */
            printf("That was not a number. Please try again.\n\n");
            int leftover = getchar();
            while (leftover != '\n' && leftover != EOF)
            {
                leftover = getchar();
            }
        }
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
    int secret_number = 0;

    do
    {
        printf("Please enter a number between 1 and 10: ");
        int items_read = scanf("%d", &secret_number);

        if (items_read == EOF)
        {
            // No input left, so we can never get a valid number. Stop here.
            printf("\nNo more input. Exiting.\n");
            return 1;
        }
        if (items_read == 0)
        {
            // Not a number. Throw away the rest of the line, just like above,
            // and set an invalid value so the loop asks again.
            int leftover = getchar();
            while (leftover != '\n' && leftover != EOF)
            {
                leftover = getchar();
            }
            secret_number = 0;
        }

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
 * - When a loop reads input, handle every result `scanf` can return. Bad input
 *   stays in the input buffer until you discard it, and `EOF` means no more input
 *   will ever arrive. Ignoring either one is a classic cause of infinite loops.
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
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c11 -o 6_loops 6_loops.c
./6_loops
```
