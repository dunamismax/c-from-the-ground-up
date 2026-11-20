# Command-Line Arguments

You've already been using command-line arguments without even realizing it!
When you compile a program like this:
`gcc -o my_program my_program.c`

The strings `-o`, `my_program`, and `my_program.c` are all command-line
arguments passed to the `gcc` program. They tell `gcc` what to do.

This is an incredibly powerful way to make your programs flexible and
scriptable. Instead of prompting the user for input with `scanf`, you can
have them provide the input when they run the program.

To access these arguments, we must use a different signature for our `main` function.


## The `main` Function with Arguments


`int main(int argc, char *argv[])`

Let's break this down:

1. `int argc`:
   `argc` stands for "argument count". It is an integer that holds the number
   of command-line arguments that were passed to your program. This count
   ALWAYS includes the name of the program itself as the first argument. So,
   `argc` will always be at least 1.

2. `char *argv[]`:
   `argv` stands for "argument vector" (a vector is just another name for an array).
   It is an ARRAY OF STRINGS (or more accurately, an array of character pointers).
   Each element in this array is one of the command-line arguments.

   - `argv[0]` is always the name of the program executable.
   - `argv[1]` is the first argument the user actually typed.
   - `argv[2]` is the second argument, and so on.
   - `argv[argc - 1]` is the last argument.

The C standard also guarantees that `argv[argc]` will be a `NULL` pointer.

## Converting Arguments to Numbers

Every argument arrives as a string, so the age `"30"` must be converted to an
`int`. You will often see `atoi()` ("ASCII to integer") used for this, but it
is RISKY: it cannot report a problem. `atoi("abc")` quietly returns 0,
`atoi("30abc")` returns 30, and a number too big for an `int` is undefined
behavior.

This lesson uses `strtol()` ("string to long") instead. Its second argument is
the address of a `char *`, which `strtol` sets to the first character it could
not convert:

- If that pointer still points at the start of the string, there were no
  digits at all.
- If it does not point at the `\0` that ends the string, there was junk after
  the number.

The third argument, 10, is the number base. A number too big even for a
`long` comes back as the largest `long`, which the lesson's range check
(0 to 150) rejects.

## Full Source

<!-- source: Part 2 - The Intermediate Path_ Deeper into C/15_command_line_arguments.c -->
```c
/**
 * @file 15_command_line_arguments.c
 * @brief Part 2, Lesson 15: Command-Line Arguments
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for using command-line arguments.
 * It explains the special `main` function signature that allows a program to
 * receive and process input directly from the command line when it's executed.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * You've already been using command-line arguments without even realizing it!
 * When you compile a program like this:
 * `gcc -o my_program my_program.c`
 *
 * The strings `-o`, `my_program`, and `my_program.c` are all command-line
 * arguments passed to the `gcc` program. They tell `gcc` what to do.
 *
 * This is an incredibly powerful way to make your programs flexible and
 * scriptable. Instead of prompting the user for input with `scanf`, you can
 * have them provide the input when they run the program.
 *
 * To access these arguments, we must use a different signature for our `main` function.
 *
 * --- The `main` Function with Arguments ---
 *
 * `int main(int argc, char *argv[])`
 *
 * Let's break this down:
 *
 * 1. `int argc`:
 *    `argc` stands for "argument count". It is an integer that holds the number
 *    of command-line arguments that were passed to your program. This count
 *    ALWAYS includes the name of the program itself as the first argument. So,
 *    `argc` will always be at least 1.
 *
 * 2. `char *argv[]`:
 *    `argv` stands for "argument vector" (a vector is just another name for an array).
 *    It is an ARRAY OF STRINGS (or more accurately, an array of character pointers).
 *    Each element in this array is one of the command-line arguments.
 *
 *    - `argv[0]` is always the name of the program executable.
 *    - `argv[1]` is the first argument the user actually typed.
 *    - `argv[2]` is the second argument, and so on.
 *    - `argv[argc - 1]` is the last argument.
 *    - The C standard also guarantees that `argv[argc]` will be a `NULL` pointer.
 */

#include <stdio.h>
#include <stdlib.h> // We need this for strtol() to convert a string to a number

// Note the new main function signature!
int main(int argc, char *argv[])
{
    // --- DEMONSTRATION 1: Printing all arguments ---
    printf("--- Part 1: Inspecting argc and argv ---\n");

    // Let's first see what `argc` contains.
    printf("Argument count (argc): %d\n", argc);

    // Now, let's loop through the `argv` array and print each argument.
    printf("Arguments (argv):\n");
    for (int i = 0; i < argc; i++)
    {
        printf("  argv[%d]: \"%s\"\n", i, argv[i]);
    }
    printf("\n");

    // --- DEMONSTRATION 2: Using the arguments ---
    printf("--- Part 2: A Practical Example ---\n");

    // A common task is to check if the user provided the correct number of arguments.
    // Let's create a simple greeter program that expects a name and an age.
    // We expect 3 arguments:
    // 1. The program name (e.g., ./15_command_line_arguments)
    // 2. A name (e.g., "Alice")
    // 3. An age (e.g., "30")
    if (argc != 3)
    {
        // If the count is wrong, print a "usage" message to standard error.
        // This is a standard practice for command-line tools.
        fprintf(stderr, "Usage: %s <name> <age>\n", argv[0]);
        fprintf(stderr, "Example: %s Alice 30\n", argv[0]);
        return 1; // Return 1 to indicate an error occurred.
    }

    // If we get here, we have the correct number of arguments.
    // `argv[1]` will be the name (which is already a string).
    char *name = argv[1];

    // `argv[2]` will be the age, but it's a STRING (e.g., "30").
    // We need to convert it to an integer.

    /*
     * You will often see `atoi()` ("ASCII to integer") used for this. It is
     * short, but RISKY: it has no way to report a problem. `atoi("abc")`
     * quietly returns 0, `atoi("30abc")` returns 30, and a number too big for
     * an `int` is undefined behavior. Your program cannot tell bad input from
     * a real 0.
     *
     * `strtol()` ("string to long") does the same job and lets us check it:
     * - The first argument is the string to convert.
     * - The second is the ADDRESS of a `char *` (the `&` from Lesson 10).
     *   `strtol` sets that pointer to the first character it could NOT
     *   convert. If it still points at the start, there were no digits at
     *   all. If it does not point at the `\0` that ends the string, there was
     *   junk after the number.
     * - The third is the number BASE: 10 for ordinary decimal numbers.
     *
     * It returns a `long`, a wider integer type. A number too big even for
     * that comes back as the largest `long`, which our range check rejects.
     */
    char *end = NULL;
    long parsed_age = strtol(argv[2], &end, 10);
    if (end == argv[2] || *end != '\0' || parsed_age < 0 || parsed_age > 150)
    {
        fprintf(stderr, "Error: The age must be a whole number from 0 to 150, not \"%s\".\n", argv[2]);
        return 1;
    }
    int age = (int)parsed_age; // Safe: we checked that it is between 0 and 150.

    printf("Hello, %s!\n", name);
    printf("You are %d years old.\n", age);
    printf("In 10 years, you will be %d.\n", age + 10);

    return 0; // Success!
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  Command-line arguments make your programs powerful and scriptable.
 * 2.  Use `int main(int argc, char *argv[])` to accept arguments.
 * 3.  `argc` is the total count of arguments, including the program's own name.
 * 4.  `argv` is an array of strings containing the actual arguments.
 * 5.  `argv[0]` is always the program's name.
 * 6.  Always check `argc` to ensure the user has provided the correct number of
 *     inputs before you try to access them in `argv`.
 * 7.  Arguments are always passed as strings. Convert them to numbers with
 *     `strtol()` (for integers) or `strtod()` (for decimals), and check that the
 *     whole string was a number. Avoid `atoi()` and `atof()`: they cannot tell
 *     you when the input was bad.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 15_command_line_arguments 15_command_line_arguments.c`
 *
 * 4. Run the executable WITH DIFFERENT ARGUMENTS to see what happens:
 *
 *    Try running with no arguments (this will fail our check):
 *    - On Linux/macOS:   `./15_command_line_arguments`
 *    - On Windows:       `15_command_line_arguments.exe`
 *
 *    Try running with the correct arguments:
 *    - On Linux/macOS:   `./15_command_line_arguments Bob 42`
 *    - On Windows:       `15_command_line_arguments.exe Bob 42`
 *
 *    Try running with too many arguments:
 *    - On Linux/macOS:   `./15_command_line_arguments Charlie 25 Extra`
 *
 *    Try running with an age that is not a number (this will fail our strtol check):
 *    - On Linux/macOS:   `./15_command_line_arguments Dana forty`
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c11 -o 15_command_line_arguments 15_command_line_arguments.c
./15_command_line_arguments Bob 42
```

Run it with no arguments, or with too many, to see the usage message. Run it
with an age that is not a number, as in `./15_command_line_arguments Dana forty`,
to see the `strtol` check reject it.
