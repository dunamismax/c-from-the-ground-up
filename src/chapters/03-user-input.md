# User Input

> **What you'll learn:** reading numbers and characters from the keyboard with `scanf`, why it needs `&`, and how to check that it worked.
>
> **Before you start:** Lessons 1 and 2.

So far, our programs have been one-way streets: they run and print things
without any input from the outside world. To build truly useful tools,
we need our programs to be interactive.

## The `scanf` Function

The counterpart to `printf` (for printing) is `scanf` (for scanning/reading).
The `scanf` function reads formatted input from the standard input, which is
usually your keyboard.

Like `printf`, it uses FORMAT SPECIFIERS to know what type of data to expect.
However, it has one very important new requirement we will explore.

To read input, we first need a variable to store that input in.
We DECLARE an integer variable `user_age` and INITIALIZE it to 0. The user
will provide the real value, but if reading fails, the variable still holds a
known value instead of leftover garbage from memory.

```c
int user_age = 0;
```

It's good practice to print a "prompt" to the user, so they know
what you're asking for:

```c
printf("Please enter your age (as a whole number): ");
```

Now, we call `scanf`. This is the most important part of the lesson:

```c
scanf("%d", &user_age)
```

- `"%d"`: This is the format specifier. It tells `scanf` to look for
  and read an integer from the keyboard input.

- `&user_age`: This is new! The `&` symbol is the ADDRESS-OF OPERATOR.
  It gets the memory address of the `user_age` variable.

## Why Do We Need `&`?

You must tell `scanf` *where* in memory to store the data it reads.
Simply providing `user_age` would only give `scanf` the variable's
*current value* (which is just 0 at this point). By providing `&user_age`,
you are giving it the *location* of the box, so it can put the user's
input directly into it.

## Checking That `scanf` Worked

Users do not always type what you ask for. What if they type `abc` instead of
a number? Then `scanf` cannot read an integer, and `user_age` is left unchanged.

Luckily, `scanf` tells us how it went. It RETURNS the NUMBER OF ITEMS it
successfully read and stored:

- `1` means it read our one integer. Success!
- `0` means the input did not match `%d` (for example, `abc`).
- `EOF` (a special negative value from `<stdio.h>`) means there was no input
  left at all. You can cause this by pressing Ctrl+D (Linux/macOS) or Ctrl+Z
  then Enter (Windows) instead of typing a number.

So we compare the result to 1:

```c
if (scanf("%d", &user_age) != 1)
{
    printf("Could not read a whole number. Exiting.\n");
    return 1;
}
```

This means: "if `scanf` did NOT read exactly 1 item, print a message and stop
the program." You will learn exactly how `if` and `!=` work in Lessons 4 and 5.
For now, recognize the pattern, because you should use it every time you call
`scanf`. `return 1;` ends `main` early with an exit code that signals an error.

## Reading Other Types

The pattern is the same for other data types. We just need to change
the variable type and the format specifier:

```c
double user_gpa = 0.0;
printf("Please enter your GPA (e.g., 3.8): ");
if (scanf("%lf", &user_gpa) != 1)
{
    printf("Could not read a number. Exiting.\n");
    return 1;
}
printf("Your GPA is %.2f.\n", user_gpa);
```

IMPORTANT: For `scanf`, the format specifier for a `double` is `%lf`
(which stands for "long float"). This is DIFFERENT from `printf`,
which uses `%f` for a `double`. This is a common point of confusion!

## The Leftover Newline

`scanf` can sometimes behave in unexpected ways because of how it handles
whitespace (like spaces, tabs, and newlines).

When you typed your GPA and pressed Enter, the `\n` (newline character)
was left behind in the INPUT BUFFER. A subsequent `scanf` call for a
character (`%c`) would read that leftover newline instead of waiting for
you to type a new character!

THE FIX: To fix this, we put a single space before the `%c` in the format
string: `" %c"`. That leading space tells `scanf` to "skip any and all
leading whitespace characters before reading the character". It's a crucial trick:

```c
if (scanf(" %c", &favorite_letter) != 1)
```


## Full Source

<!-- source: Part 1 - The Beginner Path_ Core Concepts/3_user_input.c -->
```c
/**
 * @file 3_user_input.c
 * @brief Part 1, Lesson 3: User Input
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson demonstrates how to make programs interactive by reading
 * input from the user's keyboard.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, our programs have been one-way streets: they run and print things
 * without any input from the outside world. To build truly useful tools,
 * we need our programs to be interactive.
 *
 * THE `scanf` FUNCTION
 * The counterpart to `printf` (for printing) is `scanf` (for scanning/reading).
 * The `scanf` function reads formatted input from the standard input, which is
 * usually your keyboard.
 *
 * Like `printf`, it uses FORMAT SPECIFIERS to know what type of data to expect.
 * However, it has one very important new requirement we will explore.
 */

#include <stdio.h>

int main(void)
{
    // --- Part 1: Reading an Integer ---

    /*
     * To read input, we first need a variable to store that input in.
     * We DECLARE an integer variable `user_age` and INITIALIZE it to 0. The user
     * will provide the real value, but if reading fails, the variable still
     * holds a known value instead of leftover garbage from memory.
     */
    int user_age = 0;

    /*
     * It's good practice to print a "prompt" to the user, so they know
     * what you're asking for.
     */
    printf("Please enter your age (as a whole number): ");

    /*
     * Now, we call `scanf`. This is the most important part of the lesson.
     *
     * - `"%d"`: This is the format specifier. It tells `scanf` to look for
     *   and read an integer from the keyboard input.
     *
     * - `&user_age`: This is new! The `&` symbol is the ADDRESS-OF OPERATOR.
     *   It gets the memory address of the `user_age` variable.
     *
     * WHY DO WE NEED `&`?
     * You must tell `scanf` *where* in memory to store the data it reads.
     * Simply providing `user_age` would only give `scanf` the variable's
     * *current value* (which is just 0 at this point). By providing `&user_age`,
     * you are giving it the *location* of the box, so it can put the user's
     * input directly into it.
     *
     * CHECKING THAT `scanf` WORKED
     * Users do not always type what you ask for. What if they type `abc`
     * instead of a number? Then `scanf` cannot read an integer, and `user_age`
     * is left unchanged.
     *
     * Luckily, `scanf` tells us how it went. Like many functions, it RETURNS a
     * value: the NUMBER OF ITEMS it successfully read and stored.
     * - `1` means it read our one integer. Success!
     * - `0` means the input did not match `%d` (for example, `abc`).
     * - `EOF` (a special negative value from <stdio.h>) means there was no input
     *   left at all. You can cause this by pressing Ctrl+D (Linux/macOS) or
     *   Ctrl+Z then Enter (Windows) instead of typing a number.
     *
     * So we compare the result to 1. The `if` statement below means: "if `scanf`
     * did NOT read exactly 1 item, print a message and stop the program." You
     * will learn exactly how `if` and `!=` ("not equal to") work in Lessons 4
     * and 5. For now, recognize the pattern, because you should use it every
     * time you call `scanf`.
     *
     * `return 1;` ends `main` early. Remember from Lesson 1 that returning 0
     * means success, so returning 1 tells the operating system something went
     * wrong.
     */
    if (scanf("%d", &user_age) != 1)
    {
        printf("Could not read a whole number. Exiting.\n");
        return 1;
    }

    // Let's print the value to confirm we received it correctly.
    printf("Thank you. You entered %d.\n", user_age);
    printf("\n"); // Add a blank line for spacing.

    // --- Part 2: Reading a Double ---

    /*
     * The pattern is the same for other data types. We just need to change
     * the variable type and the format specifier.
     *
     * IMPORTANT: For `scanf`, the format specifier for a `double` is `%lf`
     * (which stands for "long float"). This is DIFFERENT from `printf`,
     * which uses `%f` for a `double`. This is a common point of confusion!
     */
    double user_gpa = 0.0;
    printf("Please enter your GPA (e.g., 3.8): ");
    if (scanf("%lf", &user_gpa) != 1) // Use %lf for reading a double!
    {
        printf("Could not read a number. Exiting.\n");
        return 1;
    }

    printf("Your GPA is %.2f.\n", user_gpa); // Use %f for printing a double.
    printf("\n");

    // --- Part 3: A Common `scanf` Pitfall and Reading a Character ---

    /*
     * `scanf` can sometimes behave in unexpected ways because of how it handles
     * whitespace (like spaces, tabs, and newlines).
     *
     * When you typed your GPA and pressed Enter, the `\n` (newline character)
     * was left behind in the INPUT BUFFER. A subsequent `scanf` call for a
     * character (`%c`) would read that leftover newline instead of waiting for
     * you to type a new character!
     *
     * THE FIX: To fix this, we put a single space before the `%c` in the format
     * string: `" %c"`. That leading space tells `scanf` to "skip any and all
     * leading whitespace characters before reading the character". It's a crucial trick.
     */
    char favorite_letter = '?';
    printf("Please enter your favorite letter: ");
    if (scanf(" %c", &favorite_letter) != 1) // The space before %c is very important!
    {
        printf("Could not read a letter. Exiting.\n");
        return 1;
    }

    printf("Your favorite letter is '%c'.\n", favorite_letter);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you learned the basics of making your programs interactive.
 *
 * Key Takeaways:
 * - The `scanf()` function reads formatted input from the keyboard.
 * - `scanf()` requires the ADDRESS-OF OPERATOR `&` before the variable name to know
 *   *where* to store the input.
 * - Format specifiers for `scanf()` can sometimes be different from `printf()`.
 *   - For `double`: Use `%lf` with `scanf`, but `%f` with `printf`.
 * - `scanf()` can leave newline characters in the input buffer, which can cause
 *   problems. The `" %c"` trick (with a leading space) is used to solve this
 *    when reading characters.
 * - `scanf()` RETURNS the number of items it successfully read. Always check it:
 *   `if (scanf("%d", &x) != 1)` catches both bad input (`0`) and no input
 *   (`EOF`). Never use a variable that `scanf` failed to fill.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c17 -o 3_user_input 3_user_input.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./3_user_input`
 *    - On Windows (WSL): `./3_user_input`
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c17 -o 3_user_input 3_user_input.c
./3_user_input
```

## Try It Yourself

Practise what this lesson taught. Change a copy of the lesson file, or
start a new one.

1. Ask for the user's birth year and print how old they turn this year.
2. Run the program and type `abc` at the first prompt, then run it again and press Ctrl+D. Which message do you get each time, and why?
3. Remove the space in `" %c"` and run the program again. What happens at the letter prompt?

Try each one yourself before you look at a worked solution. The solutions,
with the reasoning in their comments, are in
[`exercises/03-user-input/`](https://github.com/dunamismax/c-from-the-ground-up/tree/main/exercises/03-user-input)
in the repository and
[on this lesson's page on the website](https://cfromthegroundup.com/lessons/03-user-input#solutions).
