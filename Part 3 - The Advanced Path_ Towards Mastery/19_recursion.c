/**
 * @file 19_recursion.c
 * @brief Part 3, Lesson 19: Recursion
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for recursion. It explains
 * the concept of a function calling itself, the importance of a base case,
 * and how recursion can be an elegant alternative to loops for some problems.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, when you've needed to repeat a task, you've used a loop (`for` or `while`).
 * Today, we're going to learn a powerful and elegant alternative: RECURSION.
 *
 * WHAT IS RECURSION?
 * RECURSION is the process of a function calling itself.
 *
 * Think of it like a set of Russian nesting dolls. You open a doll to find a
 * slightly smaller doll inside. You open that one to find an even smaller one,
 * and so on, until you reach the smallest, solid doll that can't be opened.
 *
 * In programming, a recursive function works the same way. It has two essential parts:
 *
 * 1. THE BASE CASE: This is the condition that STOPS the recursion. It's the
 *    "smallest doll" that doesn't call itself again. Without a base case, a
 *    recursive function would call itself forever, leading to a crash. This is
 *    the MOST IMPORTANT part of any recursive function.
 *
 * 2. THE RECURSIVE STEP: This is where the function calls itself, but with a
 *    modified argument that brings it one step closer to the base case. It's
 *    the act of "opening the doll to find the next smaller one."
 */

#include <stdio.h>

// --- Part 1: A Simple Recursive Countdown ---
// This function will count down from a number `n` to 1, then print "Blastoff!".
// It's a simple way to see the base case and recursive step in action.
void countdown(int n)
{
    // BASE CASE: If n is 0 or less, we stop the recursion.
    if (n <= 0)
    {
        printf("Blastoff!\n");
        return; // Stop.
    }

    // RECURSIVE STEP:
    printf("%d...\n", n);
    countdown(n - 1); // The function calls itself with a smaller number.
}

// --- Part 2: A Classic Example - Factorial ---
// The factorial of a non-negative integer `n`, denoted by `n!`, is the
// product of all positive integers less than or equal to `n`.
// Example: 5! = 5 * 4 * 3 * 2 * 1 = 120
//
// This problem has a natural recursive definition:
// - The factorial of 0 is 1. (Base Case)
// - The factorial of any other number `n` is `n` times the factorial of `n-1`. (Recursive Step)
//
// We use `long long` for the return type because factorials grow very quickly!
long long factorial(int n)
{
    // BASE CASE: The factorial of 0 (or 1) is 1.
    if (n <= 1)
    {
        return 1;
    }

    // RECURSIVE STEP: n multiplied by the factorial of the number below it.
    return n * factorial(n - 1);
}

// --- Part 3: The Danger - Infinite Recursion and Stack Overflow ---
// What happens if you forget the base case? The function will call itself
// endlessly. Each time a function is called, a small amount of memory is used
// on the "call stack". If the recursion never stops, it will eventually use up
// all the available stack memory, and the program will crash with an error
// called a STACK OVERFLOW.

/*
// DO NOT UNCOMMENT AND RUN THIS FUNCTION.
// It is an example of what NOT to do.
void infinite_recursion(void)
{
    printf("This will run forever... and then crash!\n");
    infinite_recursion(); // No base case, no modification. Uh oh.
}
*/

int main(void)
{
    printf("--- DEMONSTRATION 1: Countdown from 5 ---\n");
    countdown(5);

    printf("\n--- DEMONSTRATION 2: Calculating Factorial of 10 ---\n");
    int number = 10;
    long long fact_result = factorial(number);
    printf("The factorial of %d is %lld.\n", number, fact_result);

    // Let's try another one.
    number = 15;
    fact_result = factorial(number);
    printf("The factorial of %d is %lld.\n", number, fact_result);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  Recursion is a problem-solving technique where a function calls itself.
 * 2.  It's a powerful alternative to loops for problems that can be broken down
 *     into smaller, self-similar sub-problems.
 * 3.  Every recursive function MUST have a BASE CASE to prevent infinite recursion.
 * 4.  Every recursive function MUST have a RECURSIVE STEP that moves it closer
 *     to the base case.
 * 5.  Forgetting or having an incorrect base case leads to a STACK OVERFLOW crash.
 *
 * While loops are often more efficient in C, recursion can provide incredibly
 * elegant and readable solutions for certain types of problems, especially when
 * dealing with tree-like data structures we'll see later.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 19_recursion 19_recursion.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./19_recursion`
 *    - On Windows:       `19_recursion.exe`
 */