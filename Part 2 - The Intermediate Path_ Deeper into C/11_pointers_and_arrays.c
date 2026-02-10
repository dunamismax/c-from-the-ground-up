/**
 * @file 11_pointers_and_arrays.c
 * @brief Part 2, Lesson 11: Pointers and Arrays
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for the relationship
 * between pointers and arrays. It explains that an array name is a pointer
 * to the first element and introduces the concept of POINTER ARITHMETIC.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * In the previous lesson, you learned what a POINTER is: a variable that stores
 * a memory address. Now, we will explore one of the most important relationships
 * in C: the connection between pointers and arrays.
 *
 * THE BIG REVEAL:
 * The name of an array, by itself, acts as a CONSTANT POINTER to the first
 * element of that array.
 *
 * This means an array's name IS a memory address. This is why we didn't need the
 * `&` operator when using `scanf` with a string (which is a `char` array) back
 * in Lesson 3. The array's name already provided the address!
 *
 * This relationship enables a powerful technique called POINTER ARITHMETIC, which
 * allows us to move between elements of an array using a pointer.
 */

#include <stdio.h>

int main(void)
{
    // Let's create an array of integers.
    // In memory, these values are stored right next to each other in a
    // single, unbroken block. This is called CONTIGUOUS memory.
    int grades[] = {85, 92, 78, 99, 67};
    int count = 5; // Number of elements in the array

    // --- Part 1: The Array Name is a Pointer ---

    printf("--- Part 1: The Array Name as a Pointer ---\n");

    // Let's prove that the array name 'grades' is just an address.
    // We will print the value of 'grades' and the address of its first element.
    printf("Value of 'grades' (the array name): %p\n", grades);
    printf("Address of the first element (&grades[0]): %p\n", &grades[0]);
    printf("They are the same!\n\n");

    // --- Part 2: Pointer Arithmetic ---

    printf("--- Part 2: Pointer Arithmetic ---\n");

    // Since the array name is a pointer, we can assign it to a real pointer variable.
    int *p_grades = grades; // Note: no `&` is needed!

    // Let's access the first element using the pointer.
    printf("First element using grades[0]: %d\n", grades[0]);
    printf("First element using *p_grades: %d\n\n", *p_grades);

    // Now for the magic. What happens if we add 1 to the pointer?
    // POINTER ARITHMETIC is smart. Adding `1` to an `int` pointer doesn't just
    // add 1 to the address. It moves the address forward by the size of ONE ELEMENT.
    // In this case, it moves forward by `sizeof(int)` bytes.

    // `*(p_grades + 1)` means "go to the address of the first element, move
    // forward by the size of one integer, and then dereference that new address."
    printf("Second element using grades[1]: %d\n", grades[1]);
    printf("Second element using *(p_grades + 1): %d\n\n", *(p_grades + 1));

    printf("Third element using grades[2]: %d\n", grades[2]);
    printf("Third element using *(p_grades + 2): %d\n\n", *(p_grades + 2));

    // --- Part 3: Equivalence of p[i] and *(p + i) ---

    printf("--- Part 3: Equivalence of Subscript and Pointer Notation ---\n");

    // The C language guarantees that for any pointer or array `p` and integer `i`,
    // the expression `p[i]` is EXACTLY THE SAME as `*(p + i)`.
    // The square bracket notation `[]` is just "syntactic sugar" to make pointer
    // arithmetic look cleaner and more familiar.

    printf("Accessing the fourth element (index 3):\n");
    printf("Using array notation grades[3]: %d\n", grades[3]);
    printf("Using pointer notation *(grades + 3): %d\n\n", *(grades + 3));

    // This even works on our pointer variable `p_grades`!
    printf("Accessing the fifth element (index 4) via our pointer variable:\n");
    printf("Using pointer notation *(p_grades + 4): %d\n", *(p_grades + 4));
    printf("Using array notation p_grades[4]: %d\n\n", p_grades[4]);

    // --- Part 4: Looping Through an Array with a Pointer ---

    printf("--- Part 4: Looping Through The Array ---\n");
    printf("Grades: ");
    for (int i = 0; i < count; i++)
    {
        // We will use pointer arithmetic to access each element
        printf("%d ", *(grades + i));
    }
    printf("\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  The name of an array is a constant pointer to its first element.
 * 2.  `array_name` and `&array_name[0]` evaluate to the same memory address.
 * 3.  POINTER ARITHMETIC lets you move between elements. Adding `n` to a pointer
 *     moves it forward by `n * sizeof(element_type)` bytes in memory.
 * 4.  The standard array subscript notation `array[i]` is just a more convenient
 *     way of writing the pointer arithmetic expression `*(array + i)`. They are
 *     functionally identical.
 * 5.  This deep relationship is what makes passing arrays to functions so efficient
 *     in C—you're just passing a single address, not copying the whole array.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 11_pointers_and_arrays 11_pointers_and_arrays.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./11_pointers_and_arrays`
 *    - On Windows:       `11_pointers_and_arrays.exe`
 */