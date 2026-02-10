/**
 * @file 13_dynamic_memory_allocation.c
 * @brief Part 2, Lesson 13: Dynamic Memory Allocation
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for dynamic memory allocation.
 * It explains the concepts of the stack vs. the heap, and introduces the
 * functions `malloc()`, `free()`, and the `sizeof` operator.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Until now, the size of all our variables and arrays had to be known when we
 * wrote the program (at COMPILE-TIME). For example, `int numbers[10];` creates
 * an array that can hold exactly 10 integers. It can't hold 9, and it can't hold 11.
 *
 * But what if we don't know how much memory we need until the program is running?
 * What if we want to ask the user "How many students do you want to enter?" and
 * then create an array of just the right size?
 *
 * This is where DYNAMIC MEMORY ALLOCATION comes in. It's the ability to request
 * memory from the operating system at RUNTIME.
 *
 * THE STACK AND THE HEAP
 * Local variables (like `int x;` inside `main`) are stored on a memory region
 * called the STACK. The STACK is fast and memory is managed for you automatically.
 * When a function ends, its variables on the stack are destroyed.
 *
 * Dynamically allocated memory comes from a different, large pool of memory
 * called the HEAP. The HEAP is more flexible, but there's a catch:
 * YOU are responsible for managing it. When you are done with memory from the
 * HEAP, you MUST give it back to the system yourself.
 */

#include <stdio.h>
#include <stdlib.h> // `malloc` and `free` are declared in the Standard Library header

// --- Part 1: `malloc` - Memory Allocation ---
// The `malloc()` function is used to request a block of memory from the HEAP.
//
// - It takes one argument: the number of BYTES you want to allocate.
// - It returns a "void pointer" (`void*`) to the start of that memory block.
//   A `void*` is a generic pointer; we must CAST it to the specific pointer
//   type we need (e.g., `int*`, `char*`).
// - If `malloc()` cannot find enough free memory, it returns `NULL`. You MUST
//   always check for this!

// --- Part 2: `sizeof` - Getting The Size of a Type ---
// How do we know how many bytes an `int` or a `struct` takes up? The size can
// be different on different computers. The `sizeof` operator solves this.
// `sizeof(int)` will give us the number of bytes for a single integer.
// This makes our code portable and readable.
//
// The common pattern for `malloc` is:
// pointer = (type *)malloc(number_of_elements * sizeof(type));

// --- Part 3: `free` - Giving Memory Back ---
// When you are finished with memory you allocated with `malloc()`, you MUST
// release it using the `free()` function.
//
// If you don't, you create a MEMORY LEAK. The program holds onto memory it
// no longer needs. In a long-running program, this can eventually use up all
// available memory and crash the program or even the system.
//
// `free()` takes one argument: the pointer you got from `malloc()`.

int main(void)
{
    int num_students;
    double *gpa_list = NULL; // A pointer to hold the address of our dynamic array.
                             // It's good practice to initialize it to NULL.

    printf("How many student GPAs do you want to store? ");
    scanf("%d", &num_students);

    // Let's prevent the user from entering a non-positive number.
    if (num_students <= 0)
    {
        printf("Invalid number of students. Exiting.\n");
        return 1; // Exit with an error code.
    }

    // --- DEMONSTRATION: The `malloc`, use, `free` cycle ---

    // 1. ALLOCATE: Request memory from the heap.
    // We want space for `num_students` doubles.
    printf("Allocating memory for %d doubles...\n", num_students);
    gpa_list = (double *)malloc(num_students * sizeof(double));

    // 2. VALIDATE: ALWAYS check if malloc succeeded.
    if (gpa_list == NULL)
    {
        printf("Error: Memory allocation failed!\n");
        printf("The operating system could not provide the requested memory.\n");
        return 1; // Exit with an error code.
    }
    printf("Memory allocated successfully at address: %p\n\n", gpa_list);

    // 3. USE: Now we can use `gpa_list` just like a regular array.
    // Let's fill it with some sample data.
    for (int i = 0; i < num_students; i++)
    {
        gpa_list[i] = 2.5 + (i * 0.2); // Just some dummy values
    }

    // And print it back out to prove it works.
    printf("The stored GPAs are:\n");
    for (int i = 0; i < num_students; i++)
    {
        printf("Student %d GPA: %.2f\n", i + 1, gpa_list[i]);
    }
    printf("\n");

    // 4. FREE: We are done with the memory. We MUST return it.
    printf("Freeing the allocated memory...\n");
    free(gpa_list);

    // IMPORTANT: After freeing, the `gpa_list` pointer is now a DANGLING POINTER.
    // It points to memory that we no longer own. Using it can cause a crash.
    // It's a critical safety practice to set the pointer to NULL immediately
    // after freeing it to prevent accidental use.
    gpa_list = NULL;

    printf("Memory has been released and pointer has been set to NULL.\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  DYNAMIC MEMORY ALLOCATION allows you to request memory at RUNTIME from the HEAP.
 * 2.  The `malloc()` function allocates a block of bytes and returns a pointer to it.
 * 3.  The `sizeof` operator should always be used with `malloc` to ensure you
 *     request the correct number of bytes, making your code portable.
 * 4.  You MUST check if `malloc` returned `NULL`, which indicates allocation failure.
 * 5.  When you are done with the memory, you MUST release it with `free()`. Failure
 *     to do so results in a MEMORY LEAK.
 * 6.  After calling `free(ptr)`, set `ptr = NULL;` to prevent using a
 *     DANGLING POINTER, which is a common and dangerous bug.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 13_dynamic_memory_allocation 13_dynamic_memory_allocation.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./13_dynamic_memory_allocation`
 *    - On Windows:       `13_dynamic_memory_allocation.exe`
 */