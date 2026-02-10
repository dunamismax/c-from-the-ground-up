/**
 * @file 32_linking_external_libraries.c
 * @brief Part 5, Lesson 32: Linking External Libraries
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for linking against
 * external, pre-compiled libraries. It explains the core concepts
 * through structured comments and provides a runnable example.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * In our last lesson, we learned how to build a project from multiple source
 * files that WE wrote. But what if we want to use code written by someone else?
 * This is where LIBRARIES come in.
 *
 * WHAT IS AN EXTERNAL LIBRARY?
 * A LIBRARY is a collection of pre-written, pre-compiled code that provides
 * specific functionality. For example, there are libraries for advanced math,
 * graphics, networking, sound, and much more. Using a library saves you from
 * having to "reinvent the wheel."
 *
 * A library typically consists of two parts:
 * 1. HEADER FILES (`.h`): These contain the function declarations, just like
 *    our `helper.h` did. They tell our compiler WHAT functions are available.
 * 2. A BINARY LIBRARY FILE (`.so` on Linux, `.dylib` on macOS, `.dll` on Windows):
 *    This is the pre-compiled code. It contains the actual function definitions.
 *
 * THE LINKER
 * The LINKER is a crucial part of the compilation process. After the compiler
 * turns your source code into object files, the linker's job is to connect
 * everything. It resolves the function calls in your code to the function
 * definitions, whether they are in your own object files (like in lesson 31)
 * or in an external library file (like in this lesson).
 */

// --- Part 1: Including the Header ---

// To use a library, you must first include its header file. The C standard
// library provides a powerful math library. Its header is `<math.h>`.
// This gives us access to declarations for functions like `sqrt()` (square root)
// and `pow()` (power).
#include <stdio.h>
#include <math.h>

// --- Part 2: The Main Function ---

int main(void)
{
    // The code below uses functions declared in `<math.h>`.
    // However, including the header only tells the compiler that these functions
    // exist. It doesn't tell the linker WHERE to find the compiled code for them.
    // We'll solve that problem during compilation.

    // --- Using sqrt() ---
    // The `sqrt()` function takes a `double` and returns its square root.
    double number_for_sqrt = 81.0;
    double square_root_result = sqrt(number_for_sqrt);

    printf("Using sqrt(): The square root of %.2f is %.2f.\n",
           number_for_sqrt, square_root_result);

    // --- Using pow() ---
    // The `pow()` function takes two `double` arguments (a base and an exponent)
    // and returns the base raised to the power of the exponent.
    double base = 2.0;
    double exponent = 10.0;
    double power_result = pow(base, exponent);

    printf("Using pow(): %.2f to the power of %.2f is %.2f.\n",
           base, exponent, power_result);

    // --- Using sin() ---
    // Let's try a trigonometric function. `sin()` takes an angle in RADIANS.
    // M_PI is a constant defined in <math.h> for the value of Pi (π).
    double angle_radians = M_PI / 2.0; // 90 degrees
    double sin_result = sin(angle_radians);

    printf("Using sin(): The sine of PI/2 radians is %.2f.\n", sin_result);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * HOW TO COMPILE AND RUN THIS CODE (THE IMPORTANT PART!):
 *
 * Because we are using functions from an external math library, we must explicitly
 * tell the linker to include that library's code in our final executable.
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler with a special flag to link the math library:
 *
 *    `gcc -Wall -Wextra -std=c11 -o 32_linking_external_libraries 32_linking_external_libraries.c -lm`
 *
 * THE -lm FLAG
 * This is the key to this lesson. The `-l` flag is used to tell the linker
 * which library to link. The name of the math library is `m` (from its file,
 * which is often `libm.so`). So, `-lm` means "link the math library".
 *
 * CONVENTION: The `-l` flags should always come at the END of your command,
 * after your source files.
 *
 * WHAT HAPPENS IF YOU FORGET `-lm`?
 * Try compiling with: `gcc ... -o output 32_linking_external_libraries.c`
 * You will get an "undefined reference to `sqrt`" error (and to `pow`, `sin`).
 * This is the linker telling you, "I see you're calling a function named `sqrt`,
 * but I have no idea where its actual code is!" The `-lm` flag solves this by
 * telling the linker to look inside the math library.
 *
 * 4. Run the executable:
 *    - On Linux/macOS:   `./32_linking_external_libraries`
 *    - On Windows:       `32_linking_external_libraries.exe`
 */