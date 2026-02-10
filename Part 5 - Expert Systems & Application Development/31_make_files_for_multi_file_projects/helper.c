/**
 * @file helper.c
 * @brief Part 5, Lesson 31: Implementation File for the Helper Module
 * @author dunamismax
 * @date 06-15-2025
 *
 * This is a SOURCE FILE that provides the implementation for the
 * functions declared in `helper.h`.
 */

/*
 * =====================================================================================
 * |                          - LESSON: SOURCE FILES -                                 |
 * =====================================================================================
 *
 * WHAT IS A SOURCE IMPLEMENTATION FILE?
 * This `.c` file contains the actual code, the DEFINITIONS, for the functions
 * that were promised (declared) in the corresponding header file (`helper.h`).
 *
 * By separating the declaration (`.h`) from the definition (`.c`), we create
 * modular, reusable code. Another programmer could use our `helper` module by
 * just reading `helper.h`; they wouldn't need to know the details of our code here.
 *
 * --- Including Local Headers ---
 * Notice we use `#include "helper.h"` with double quotes. This is a signal
 * to the compiler to look for the file in the CURRENT directory first, before
 * searching the system's standard library paths.
 *
 *  - `#include <stdio.h>`: Use angle brackets for system libraries.
 *  - `#include "helper.h"`: Use double quotes for your own project's header files.
 */

#include <stdio.h>
#include "helper.h" // We need to include our own header file.

/**
 * @brief Prints a message to demonstrate a function call from a separate file.
 *
 * This is the DEFINITION of the function that was declared in `helper.h`.
 * It contains the actual block of code that will be executed when the
 * function is called.
 */
void say_hello_from_helper(void)
{
    printf("Hello from the helper module! This message comes from helper.c.\n");
}