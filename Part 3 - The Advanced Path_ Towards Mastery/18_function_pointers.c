/**
 * @file 18_function_pointers.c
 * @brief Part 3, Lesson 18: Function Pointers
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for function pointers.
 * It explains how to declare, assign, and use pointers to functions,
 * enabling powerful techniques like callbacks and dynamic function calls.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * You have mastered pointers to DATA (like `int*` or `struct Student*`), which
 * store the memory address of a variable.
 *
 * C takes this concept one step further: you can also have pointers to CODE.
 * A FUNCTION POINTER is a variable that stores the memory address of a function.
 *
 * This might sound strange, but it's an incredibly powerful feature. It allows
 * you to treat functions like any other variable:
 * - You can pass a function as an argument to another function.
 * - You can store functions in an array.
 * - You can change which function gets called at runtime.
 *
 * The most common use case is creating CALLBACKS, where you provide a generic
 * function with a specific "helper" function for it to call back to complete its task.
 */

#include <stdio.h>
#include <stdlib.h>

// --- Part 1: The Functions We'll Point To ---
// Let's define two simple functions. It's crucial that they have the
// exact same SIGNATURE (return type and parameter types).

int add(int a, int b)
{
    return a + b;
}

int subtract(int a, int b)
{
    return a - b;
}

// --- Part 2: The Callback Function ---
// Here is a generic function that takes two numbers and a function pointer
// as arguments. It will use the provided function to perform a calculation.
//
// The third parameter `int (*operation_func)(int, int)` is the function pointer.
// It says: "I expect the address of a function that takes two ints and returns an int."
void perform_calculation(int x, int y, int (*operation_func)(int, int))
{
    // First, check if the pointer is valid (not NULL)
    if (operation_func == NULL)
    {
        printf("Error: No operation function provided.\n");
        return;
    }
    int result = operation_func(x, y); // Call the function through the pointer
    printf("Performing calculation... Result is: %d\n", result);
}

int main(void)
{
    // --- DEMONSTRATION 1: Declaring and Using a Function Pointer ---
    printf("--- Part 1: Basic Function Pointer Usage ---\n");

    // This is the syntax for declaring a function pointer.
    // It must match the signature of the functions it will point to.
    // return_type (*pointer_name)(parameter_types);
    int (*p_calculate)(int, int) = NULL;

    // The parentheses around `*p_calculate` are ESSENTIAL.
    // Without them, `int *p_calculate(int, int);` would declare a
    // function named `p_calculate` that returns an `int*`.

    // ASSIGNMENT: Assign the address of the `add` function to the pointer.
    // The name of a function, like an array name, decays into a pointer.
    p_calculate = add; // You can also use `&add`, but it's not necessary.

    // CALLING: Call the function through the pointer.
    int sum = p_calculate(10, 5);
    printf("Calling via pointer (add): %d\n", sum);

    // Now, point the SAME pointer to a DIFFERENT function.
    p_calculate = subtract;

    // And call it again. The pointer now invokes different code.
    int difference = p_calculate(10, 5);
    printf("Calling via pointer (subtract): %d\n\n", difference);

    // --- DEMONSTRATION 2: Using Function Pointers as Callbacks ---
    printf("--- Part 2: Callbacks in Action ---\n");

    int a = 20;
    int b = 12;

    // Here, we call `perform_calculation` and pass the `add` function
    // as the callback. `perform_calculation` will "call back" to `add`.
    printf("Passing 'add' function as a callback:\n");
    perform_calculation(a, b, add);

    // Now we call the exact same generic function, but we give it a
    // different tool to work with.
    printf("Passing 'subtract' function as a callback:\n");
    perform_calculation(a, b, subtract);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  A FUNCTION POINTER stores the memory address of a function.
 * 2.  The declaration syntax is critical: `return_type (*pointer_name)(params);`.
 *     The parentheses around the pointer name are mandatory.
 * 3.  You can assign the name of a compatible function directly to the pointer.
 * 4.  This allows for incredibly flexible code, primarily by enabling CALLBACKS.
 * 5.  A callback is when you pass a function (A) as an argument to another
 *     function (B), and B then calls A to perform a specific part of its task.
 *     This allows you to customize the behavior of generic functions.
 *
 * This is an advanced topic, but mastering it opens the door to designing
 * highly modular and reusable C programs.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 18_function_pointers 18_function_pointers.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./18_function_pointers`
 *    - On Windows:       `18_function_pointers.exe`
 */