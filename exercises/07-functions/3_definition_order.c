/**
 * @file 3_definition_order.c
 * @brief Lesson 7, Exercise 3: what happens, and why
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Move a function's definition above `main` and delete its prototype. Does it
 * still compile? Now move it below `main` again, still without the prototype,
 * and read the error.
 *
 * THE ANSWER:
 * Above `main`: yes, it compiles and runs, and this file is that version.
 * Below `main` with no prototype: no. The reasons are at the end.
 */

#include <stdio.h>

// No prototype here. The whole DEFINITION comes before `main` instead.
int calculate_damage(int strength, int weapon_power)
{
    int calculated_value = strength + (weapon_power * 2);
    return calculated_value;
}

int main(void)
{
    // By the time the compiler reads this call it has already seen the
    // function above, so it knows the name, the two `int` parameters and
    // the `int` return type.
    int total_damage = calculate_damage(20, 8);

    printf("Total calculated damage is: %d\n", total_damage);

    return 0;
}

/*
 * WHY ORDER MATTERS
 *
 * The compiler reads a file once, from top to bottom. When it reaches a
 * function call it must already know what that function takes and returns.
 * A definition tells it everything a prototype would, so a definition
 * placed above the call is enough.
 *
 * Move `calculate_damage` below `main` and the call comes first. GCC then
 * stops with:
 *
 *   error: implicit declaration of function 'calculate_damage'
 *
 * and Clang with:
 *
 *   error: call to undeclared function 'calculate_damage'; ISO C99 and later
 *   do not support implicit function declarations
 *
 * Both mean: "you called something I have not been told about yet". (Very
 * old compilers only warned, guessed that the function returns `int`, and
 * carried on. A wrong guess made the program misbehave when it ran.)
 *
 * SO WHY WRITE PROTOTYPES AT ALL?
 *
 * - With prototypes at the top, the definitions can go in any order, and
 *   `main` can come first, where a reader expects to start.
 * - Two functions that call each other cannot BOTH be defined first.
 * - In programs made of several files (Lesson 31), prototypes in a header
 *   file are how one file learns about functions defined in another.
 *
 * Strict compilers can even ask for them: with the extra flag
 * `-Wmissing-prototypes`, GCC and Clang warn about this file ("no previous
 * prototype for 'calculate_damage'"). The course's own checks use that
 * flag, which is why every lesson lists its prototypes.
 *
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 3_definition_order 3_definition_order.c
 *   ./3_definition_order
 */
