/**
 * @file 2_three_divisions.c
 * @brief Lesson 4, Exercise 2: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Print `7 / 2`, `7.0 / 2` and `(double)7 / 2`. Explain each result.
 */

#include <stdio.h>

int main(void)
{
    // Both operands are integers, so this is INTEGER DIVISION. The exact
    // answer, 3.5, loses its fractional part and becomes 3. The result is
    // an `int`, so it is printed with `%d`.
    printf("7 / 2         = %d\n", 7 / 2);

    // `7.0` is written with a decimal point, which makes it a `double`.
    // When one operand is a `double`, C converts the other one too and
    // divides them as floating-point numbers: 3.5. The result is a
    // `double`, so it is printed with `%f`.
    printf("7.0 / 2       = %f\n", 7.0 / 2);

    // `(double)7` is a CAST: it converts the integer 7 to the `double` 7.0
    // before the division happens. From there it is the same as the line
    // above.
    printf("(double)7 / 2 = %f\n", (double)7 / 2);

    return 0;
}

/*
 * THE RULE
 *
 * The TYPES of the two operands decide which division you get, not the
 * variable the result is stored in and not the format specifier you print
 * it with. Two integers divide as integers. If either one is a `double`,
 * the division is done in floating point.
 *
 * With literal numbers you can simply write `7.0`. The cast matters when the
 * numbers are in `int` variables, as with `(double)a / b` in the lesson.
 *
 * ONE TRAP
 *
 * A cast applies to the value right after it, so `(double)7 / 2` converts
 * the 7 and then divides. Write `(double)(7 / 2)` instead and the
 * parentheses make the integer division happen first: 7 / 2 is 3, and only
 * then is that 3 converted, giving 3.0. The fraction was already gone.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 2_three_divisions 2_three_divisions.c
 *   ./2_three_divisions
 */
