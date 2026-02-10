/**
 * @file 24_static_and_extern_variables.c
 * @brief Part 3, Lesson 24: Static and Extern Variables
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for storage classes,
 * specifically the `static` and `extern` keywords. It explains how these
 * control a variable's lifetime, scope, and linkage across files.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * As programs grow, we need more control over our variables and functions. We need
 * to answer two key questions:
 *
 * 1. LIFETIME: How long does a variable exist in memory?
 * 2. LINKAGE (VISIBILITY): Which parts of our code are allowed to see it?
 *
 * The `static` and `extern` keywords are our tools for managing this.
 *
 * `static` - The "Private" Keyword
 * The `static` keyword has two different meanings depending on where you use it,
 * but both are related to making things more "private" or "persistent".
 *
 * `extern` - The "Public Declaration" Keyword
 * The `extern` keyword is used to DECLARE a global variable that is DEFINED in
 * another file. It tells the compiler, "Don't worry, this variable exists somewhere
 * else. The linker will find it and connect everything."
 */

#include <stdio.h>

// --- Part 1: `static` Inside a Function (Controlling LIFETIME) ---
// When used inside a function, `static` changes a variable's storage duration.
// - A normal (auto) local variable is created on the stack when the function is
//   called and destroyed when the function exits.
// - A `static` local variable is created only once and exists for the ENTIRE
//   lifetime of the program. It RETAINS ITS VALUE between function calls.

void regular_counter()
{
    int count = 0; // This variable is recreated and reset to 0 every time.
    count++;
    printf("Regular counter is at: %d\n", count);
}

void static_counter()
{
    // This variable is initialized to 0 only the VERY FIRST time.
    // In all subsequent calls, it keeps its previous value.
    static int count = 0;
    count++;
    printf("Static counter is at: %d\n", count);
}

// --- Part 2: `static` at File Scope (Controlling LINKAGE) ---
// When used on a global variable or a function, `static` changes its linkage
// from "external" to "internal".
// - An EXTERNAL linkage variable (the default) can be seen and used by other .c files.
// - An INTERNAL linkage variable is private to this file. No other .c file can see it.
//   This is a key tool for ENCAPSULATION, or hiding implementation details.

int g_public_variable = 100; // EXTERNAL linkage. Visible to other files.

static int g_private_variable = 42; // INTERNAL linkage. Private to this file.

// This also applies to functions.
static void private_helper_function()
{
    printf("This is a private helper function. It cannot be called from other files.\n");
}

// --- Part 3: `extern` for Sharing Globals ---
// Imagine we have a variable defined in another file, `globals.c`:
//
//   // In globals.c:
//   int g_shared_counter = 500;
//
// To use `g_shared_counter` in this file, we must DECLARE it with `extern`.
// This tells the compiler that we intend to use it, but the actual variable
// is defined elsewhere. For this single-file example, we will define it at
// the bottom of this file to show how the declaration works.

extern int g_late_defined_variable; // A DECLARATION, not a definition.

int main(void)
{
    printf("--- Part 1: `static` for Variable Lifetime ---\n");
    printf("Calling counters three times:\n");
    regular_counter();
    static_counter();
    printf("---\n");
    regular_counter();
    static_counter();
    printf("---\n");
    regular_counter();
    static_counter();
    printf("\n");

    printf("--- Part 2: `static` for File-Scope Linkage ---\n");
    printf("The public global variable is: %d\n", g_public_variable);
    printf("The private static variable is: %d\n", g_private_variable);
    private_helper_function();
    printf("\n");

    printf("--- Part 3: Using an `extern` Variable ---\n");
    printf("The late-defined variable declared with 'extern' is: %d\n", g_late_defined_variable);

    return 0;
}

// This is the DEFINITION for the `extern` variable declared earlier.
// Because the `main` function has already seen the `extern` declaration, this works.
// In a real project, this definition would be in a separate .c file.
int g_late_defined_variable = 999;

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1. The `static` keyword has two meanings based on context:
 *    - INSIDE a function: Creates a variable that persists for the program's
 *      entire lifetime and retains its value between calls.
 *    - OUTSIDE a function: Creates a global variable or function that is private
 *      to the current file (internal linkage).
 *
 * 2. By default, global variables and functions have EXTERNAL linkage, meaning
 *    they can be shared across multiple .c files.
 *
 * 3. The `extern` keyword is used to DECLARE a global variable that is DEFINED
 *    in another file, allowing you to share variables across your project.
 *
 * These concepts are the bedrock of creating well-structured, multi-file C projects.
 * You use `static` to hide details within a file and `extern` (usually via a header file)
 * to expose the public parts.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 24_static_and_extern_variables 24_static_and_extern_variables.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./24_static_and_extern_variables`
 *    - On Windows:       `24_static_and_extern_variables.exe`
 */