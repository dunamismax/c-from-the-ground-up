/**
 * @file 4_basic_operators.c
 * @brief Part 1, Lesson 4: Basic Operators
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson covers the fundamental operators in C used for performing
 * calculations and making comparisons.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * In programming, an OPERATOR is a special symbol that performs an operation on
 * one or more values or variables (called OPERANDS). For example, in `5 + 3`,
 * the `+` is the operator and `5` and `3` are the operands.
 *
 * We will explore three main categories of operators in this lesson:
 * 1. Arithmetic Operators: For performing mathematical calculations.
 * 2. Relational Operators: For comparing two values.
 * 3. Logical Operators: For combining multiple comparisons.
 */

#include <stdio.h>

int main(void)
{
    // Let's declare some variables to work with.
    int a = 10;
    int b = 3;

    // --- Part 1: Arithmetic Operators ---

    printf("--- Part 1: Arithmetic Operators ---\n");
    printf("Initial values: a = %d, b = %d\n\n", a, b);

    // `+` (Addition)
    printf("a + b = %d\n", a + b);

    // `-` (Subtraction)
    printf("a - b = %d\n", a - b);

    // `*` (Multiplication)
    printf("a * b = %d\n", a * b);

    /*
     * `/` (Division) - This one is tricky!
     * When you divide two integers in C, the result is also an integer.
     * The fractional part is simply thrown away (truncated). This is called
     * INTEGER DIVISION.
     */
    printf("Integer Division (a / b) = %d\n", a / 3); // 10 / 3 is 3, not 3.333

    /*
     * To get a precise decimal result, at least one of the operands must be a
     * floating-point type (like `double`). We can achieve this by "casting"
     * one of the integers to a double.
     */
    double precise_division = (double)a / b;
    printf("Floating-Point Division ((double)a / b) = %f\n", precise_division);

    /*
     * `%` (Modulus)
     * This operator gives you the REMAINDER of an integer division. It is
     * incredibly useful in many programming scenarios.
     */
    printf("Modulus (a %% b) = %d\n", a % b); // 10 divided by 3 is 3 with a remainder of 1.
    // Note: To print a literal '%' character in printf, you must type '%%'.
    printf("\n");

    // --- Part 2: Relational Operators ---

    /*
     * RELATIONAL OPERATORS are used to compare two values. The result of a
     * relational operation is a "truth" value. In C, there is no built-in
     * boolean type like `true` or `false`. Instead, C uses integers:
     * - `0` represents FALSE.
     * - Any non-zero integer represents TRUE (typically `1`).
     */
    printf("--- Part 2: Relational Operators ---\n");
    printf("The results below will be 1 for TRUE and 0 for FALSE.\n\n");

    // `==` (Equal to) - VERY IMPORTANT:
    // A single `=` is for assignment. A double `==` is for comparison.
    // This is one of the most common beginner mistakes!
    printf("Is a == 10? %d\n", a == 10); // True (1)
    printf("Is a == b?  %d\n", a == b);  // False (0)

    // `!=` (Not equal to)
    printf("Is a != b?  %d\n", a != b); // True (1)

    // `>` (Greater than)
    printf("Is a > b?   %d\n", a > b); // True (1)

    // `<` (Less than)
    printf("Is a < b?   %d\n", a < b); // False (0)

    // `>=` (Greater than or equal to)
    printf("Is a >= 10? %d\n", a >= 10); // True (1)

    // `<=` (Less than or equal to)
    printf("Is b <= 3?  %d\n", b <= 3); // True (1)
    printf("\n");

    // --- Part 3: Logical Operators ---

    /*
     * LOGICAL OPERATORS are used to combine or modify logical (true/false) expressions.
     */
    printf("--- Part 3: Logical Operators ---\n");
    int is_player_alive = 1; // 1 for true
    int has_key = 0;         // 0 for false
    printf("Player is alive: %d, Player has key: %d\n\n", is_player_alive, has_key);

    // `&&` (Logical AND) - evaluates to true ONLY if BOTH operands are true.
    printf("Can open door (is_player_alive && has_key)? %d\n", is_player_alive && has_key);

    // `||` (Logical OR) - evaluates to true if AT LEAST ONE operand is true.
    printf("Is player alive OR has key? %d\n", is_player_alive || has_key);

    // `!` (Logical NOT) - inverts the truth value of its operand.
    printf("The opposite of has_key (!has_key) is: %d\n", !has_key);
    printf("\n");

    // --- Part 4: Operator Precedence ---
    printf("--- Part 4: Operator Precedence ---\n");
    /*
     * Just like in math (PEMDAS/BODMAS), operators in C have a default order
     * of evaluation called OPERATOR PRECEDENCE. For example, `*` and `/` are
     * evaluated before `+` and `-`.
     */
    int result1 = 5 + 2 * 10;             // 2*10 is done first, then 5 is added.
    printf("5 + 2 * 10 = %d\n", result1); // Prints 25, not 70.

    /*
     * You can use parentheses `()` to force a specific order of evaluation,
     * just like in math. It's often a good idea to use parentheses to make
     * your code clearer, even when not strictly necessary.
     */
    int result2 = (5 + 2) * 10;             // (5+2) is done first.
    printf("(5 + 2) * 10 = %d\n", result2); // Prints 70.

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you learned about C's fundamental operators:
 * - Arithmetic (`+`, `-`, `*`, `/`, `%`) for calculations.
 * - Relational (`==`, `!=`, `>`, `<`, `>=`, `<=`) for comparisons.
 * - Logical (`&&`, `||`, `!`) for combining true/false conditions.
 *
 * You also learned about integer division, the modulus operator, and how to
 * use parentheses `()` to control the order of operations.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 4_basic_operators 4_basic_operators.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./4_basic_operators`
 *    - On Windows:       `4_basic_operators.exe`
 */