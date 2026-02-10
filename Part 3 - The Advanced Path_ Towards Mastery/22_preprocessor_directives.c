/**
 * @file 22_preprocessor_directives.c
 * @brief Part 3, Lesson 22: Preprocessor Directives
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for the C preprocessor.
 * It explains key directives like #include, #define, and conditional
 * compilation, which are essential for managing larger codebases.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Before your C code is ever seen by the actual compiler, it goes through a
 * first pass by a program called the C PREPROCESSOR. The preprocessor's job is
 * simple: it's a text-replacement tool that scans your code for lines starting
 * with a hash symbol (`#`) and acts on them. These are called PREPROCESSOR DIRECTIVES.
 *
 * You've been using these since day one without necessarily knowing the details.
 * Let's explore the most important ones.
 */

// --- Part 1: `#include` Revisited ---
// You know that `#include` pastes the contents of another file into this one.
// But there's a subtle difference between the two forms:

#include <stdio.h> // `<...>` : Use for standard system libraries. The preprocessor
                   //           searches for these in a list of standard system directories.

// #include "my_header.h" // `"..."` : Use for your own header files. The preprocessor
//           searches for this file first in the CURRENT directory,
//           and then in the standard system directories.
// This is crucial for multi-file projects.

// --- Part 2: `#define` for Constants and Macros ---
// `#define` is used to create MACROS. A macro is a fragment of code that has
// been given a name. Whenever the name is used, it is replaced by the contents
// of the macro. This is direct text substitution.

// 1. Object-like Macros (Constants)
// This tells the preprocessor: "anywhere you see PI, replace it with 3.14159".
// By convention, macro constants are in ALL_CAPS.
#define PI 3.14159
#define GREETING "Hello, World!"

// 2. Function-like Macros
// These macros can take arguments.
// THE PARENTHESES RULE: When creating function-like macros, ALWAYS surround
// each argument and the ENTIRE expression with parentheses. This prevents
// operator precedence bugs after the text substitution happens.

// BAD EXAMPLE:
#define BAD_SQUARE(x) x *x
// If you call `BAD_SQUARE(2 + 3)`, it expands to `2 + 3 * 2 + 3`, which is `2 + 6 + 3 = 11`. WRONG!

// GOOD EXAMPLE (Follow this rule!):
#define SQUARE(x) ((x) * (x))
// Now, `SQUARE(2 + 3)` expands to `((2 + 3) * (2 + 3))`, which is `5 * 5 = 25`. CORRECT!

// --- Part 3: Conditional Compilation ---
// These directives allow you to include or exclude parts of your code from
// compilation based on certain conditions. This is extremely useful.

// A common use is for debugging messages. We can define a `DEBUG` macro.
#define DEBUG

int main(void)
{
    printf("--- Part 1 & 2: Using #define Macros ---\n");
    double radius = 5.0;
    double area = PI * SQUARE(radius); // Preprocessor expands this before compilation

    printf("%s\n", GREETING);
    printf("The area of a circle with radius %.2f is %.2f\n", radius, area);

    int val = 2 + 3;
    printf("The square of (2 + 3) is %d\n\n", SQUARE(val));

    printf("--- Part 3: Conditional Compilation in Action ---\n");

// This block of code will only be included if the DEBUG macro is defined.
#ifdef DEBUG
    printf("DEBUG: This is a debug message.\n");
    printf("DEBUG: The value of 'area' is %f.\n", area);
#endif

// You can also use #ifndef (if not defined).
#ifndef RELEASE_MODE
    printf("This is not a release build.\n");
#endif

// You can also check values with #if
#if (10 > 5)
    printf("#if (10 > 5) is true, so this line is compiled.\n");
#endif

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  The PREPROCESSOR runs before the compiler, performing text-based operations
 *     on your code based on directives starting with `#`.
 * 2.  `#include <...>` is for system libraries; `#include "..."` is for your own files.
 * 3.  `#define` creates macros for constants or simple functions. ALWAYS use parentheses
 *     in function-like macros to avoid bugs: `#define NAME(arg) ((arg)...)`.
 * 4.  Conditional compilation (`#ifdef`, `#ifndef`, `#if`, `#else`, `#endif`) lets you
 *     include or exclude code, which is perfect for debugging or platform-specific features.
 *
 * THE MOST IMPORTANT USE OF CONDITIONAL COMPILATION: HEADER GUARDS
 *
 * When you start creating your own `.h` files, you'll run into a "double inclusion"
 * problem. To prevent this, every header file you create MUST be wrapped in
 * a "header guard", which looks like this:
 *
 *   #ifndef MY_UNIQUE_HEADER_NAME_H
 *   #define MY_UNIQUE_HEADER_NAME_H
 *
 *   // All your header file content goes here...
 *
 *   #endif // MY_UNIQUE_HEADER_NAME_H
 *
 * This ensures that even if the file is `#include`d ten times, its content is
 * only processed by the compiler ONCE. We will use this in future lessons.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 22_preprocessor_directives 22_preprocessor_directives.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./22_preprocessor_directives`
 *    - On Windows:       `22_preprocessor_directives.exe`
 *
 * To see conditional compilation work, try commenting out `#define DEBUG` at the
 * top, then recompile and run. The debug messages will disappear!
 *
 * Alternatively, you can define macros from the command line with the -D flag.
 * Remove `#define DEBUG` from the file, then compile like this:
 * `gcc -Wall -Wextra -std=c11 -DDEBUG -o 22_preprocessor_directives 22_preprocessor_directives.c`
 * The debug messages will reappear!
 */