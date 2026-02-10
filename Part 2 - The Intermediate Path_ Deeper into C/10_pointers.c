/**
 * @file 10_pointers.c
 * @brief Part 2, Lesson 10: Pointers
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for pointers.
 * It explains the core concepts of memory addresses, pointer declaration,
 * the address-of operator, and the dereference operator through structured
 * comments and provides a runnable example of their implementation.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Welcome to Part 2 of your C journey! You've mastered the fundamentals, and now
 * we're diving deeper. This lesson introduces POINTERS, arguably the most
 * powerful and unique feature of the C language.
 *
 * Don't be intimidated! The concept is simple:
 * Instead of holding a value (like 5, or 'c'), a POINTER holds a memory ADDRESS.
 * It's like having a note that says "the data you want is over there" instead of
 * holding the data itself.
 *
 * WHY ARE POINTERS IMPORTANT?
 * Pointers allow for highly efficient code and are essential for many advanced
 * topics, including dynamic memory allocation, building complex data structures
 * (like the linked lists we'll see later), and allowing functions to modify
 * the original variables you pass to them. Mastering pointers is mastering C.
 */

#include <stdio.h> // We need this for printf and to use NULL

// --- Part 1: Variables and Their Memory Addresses ---

// Every variable you declare is stored somewhere in your computer's memory (RAM).
// Each location in memory has a unique ADDRESS, like a house number on a street.

// The ADDRESS-OF operator (`&`) gives you the memory address of a variable.

// --- Part 2: What is a Pointer? ---

// A POINTER is a special kind of variable that is designed to store a memory ADDRESS.

// DECLARING A POINTER:
// You declare a pointer by specifying the type of data it will point to,
// followed by an asterisk (*), and then the pointer's name.
//
// SYNTAX: data_type *pointer_name;
//
// Example: `int *p_number;` declares a pointer named `p_number` that is
//          intended to hold the address of an `int` variable.
//
// It's a very good practice to initialize pointers to NULL. NULL is a special
// value that means "this pointer doesn't point to anything valid." This helps
// prevent bugs from using a pointer that points to a random, garbage address.

// --- Part 3: The Dereference Operator (*) ---

// The asterisk `*` has a second job. When used with an existing pointer variable
// (not in its declaration), it acts as the DEREFERENCE operator.
//
// DEREFERENCING means "go to the address stored in this pointer and get the
// value that is located there."

int main(void)
{
    // --- DEMONSTRATION 1: Getting a Variable's Address ---

    int score = 94; // A regular integer variable. It has a value (94) and an address.

    printf("--- Part 1: Variable Addresses ---\n");
    printf("The value of the 'score' variable is: %d\n", score);

    // We use the address-of operator (&) to get its memory location.
    // The `%p` format specifier is used to print addresses in a standard hexadecimal format.
    printf("The memory address of 'score' is: %p\n\n", &score);

    // --- DEMONSTRATION 2: Declaring and Using a Pointer ---

    printf("--- Part 2 & 3: Pointers in Action ---\n");

    // Declare a pointer that can hold the address of an integer.
    // We initialize it to NULL for safety.
    int *p_score = NULL;

    printf("Initial value of p_score (a safe pointer): %p\n", p_score);

    // Now, let's make the pointer 'point to' our 'score' variable.
    // We assign the address of 'score' to the pointer 'p_score'.
    p_score = &score;

    printf("Value of p_score after assignment: %p (Notice it's score's address!)\n", p_score);
    printf("Address of the pointer itself (p_score): %p\n\n", &p_score); // Pointers have their own address too!

    // --- DEMONSTRATION 3: Dereferencing a Pointer ---

    printf("--- Part 4: Dereferencing ---\n");
    // To get the value stored at the address the pointer holds, we DEREFERENCE it with *.
    printf("Value at the address p_score points to: %d\n\n", *p_score);

    // --- DEMONSTRATION 4: Modifying a Variable via its Pointer ---
    // This is where the true power of pointers starts to show. We can change the
    // original 'score' variable's value using only the pointer.

    printf("--- Part 5: Modifying Data via Pointer ---\n");
    printf("Original score: %d\n", score);

    printf("Changing value through the pointer...\n");
    *p_score = 100; // "Go to the address stored in p_score and set the value there to 100."

    // Let's check the original 'score' variable now.
    printf("New score: %d\n", score); // It has been changed!

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  Every variable has a value AND a memory ADDRESS.
 * 2.  The ADDRESS-OF operator `&` gets the memory address of a variable.
 * 3.  A POINTER is a variable that stores a memory address.
 * 4.  The DEREFERENCE operator `*` (when used on an existing pointer) goes to the
 *     pointer's stored address and retrieves the value at that location.
 * 5.  Pointers allow you to indirectly read and modify the variables they point to.
 * 6.  Initializing pointers to `NULL` is a critical safety practice.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 10_pointers 10_pointers.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./10_pointers`
 *    - On Windows:       `10_pointers.exe`
 */