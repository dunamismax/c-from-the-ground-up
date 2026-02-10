/**
 * @file 8_arrays.c
 * @brief Part 1, Lesson 8: Arrays
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson introduces arrays, a fundamental data structure for storing
 * collections of elements of the same type.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, we have only used variables that can hold one value at a time. For
 * example, `int score = 100;`. But what if you need to store the scores for
 * an entire class of 30 students? Declaring 30 separate variables (`score1`,
 * `score2`, etc.) would be extremely inefficient.
 *
 * An ARRAY is a data structure that can store a fixed-size, sequential
 * collection of elements of the SAME data type. Think of it as a row of
 * numbered boxes, where each box holds a value of the same type.
 */

#include <stdio.h>

int main(void)
{
    // --- Part 1: Declaring and Initializing an Array ---

    /*
     * To declare an array, you specify the data type of its elements, the name
     * of the array, and the number of elements it can hold (its size) in
     * square brackets `[]`.
     *
     * The following line declares an array named `high_scores` that can hold
     * exactly 5 integers.
     */
    int high_scores[5]; // An array of 5 integers.

    /*
     * ACCESSING ARRAY ELEMENTS:
     * You access individual elements of an array using an INDEX in square brackets.
     * C uses ZERO-BASED INDEXING. This is a critical concept. It means the
     * first element is at index 0, the second at index 1, and so on.
     * For an array of size 5, the valid indices are 0, 1, 2, 3, and 4.
     *
     * Accessing an index outside of this range (like `high_scores[5]`) leads to
     * UNDEFINED BEHAVIOR, a common source of bugs and crashes in C programs.
     */
    printf("--- Part 1: Assigning and Accessing Array Elements ---\n");
    high_scores[0] = 980; // Assign 980 to the first element
    high_scores[1] = 950; // Assign 950 to the second element
    high_scores[2] = 875;
    high_scores[3] = 820;
    high_scores[4] = 799; // The fifth (and last) element

    printf("The highest score is: %d\n", high_scores[0]);
    printf("The third highest score is: %d\n", high_scores[2]);
    printf("\n");

    // --- Part 2: Initializer Lists ---

    /*
     * Manually assigning each element is tedious. You can initialize an array
     * when you declare it using an initializer list in curly braces `{}`.
     *
     * If you provide an initializer list, you can let the compiler figure out
     * the size of the array by leaving the square brackets empty.
     */
    printf("--- Part 2: Using an Initializer List ---\n");
    double temperatures[] = {72.5, 75.0, 69.8, 80.1, 85.3}; // Size is 5
    char grades[] = {'A', 'B', 'A', 'C', 'B', 'A'};         // Size is 6

    printf("The first temperature recorded was: %.1f\n", temperatures[0]);
    printf("The fourth student's grade was: %c\n", grades[3]);
    printf("\n");

    // --- Part 3: Iterating Over an Array ---

    /*
     * The real power of arrays is realized when you combine them with loops.
     * A `for` loop is perfect for iterating through an array, performing an
     * action on each element.
     */
    printf("--- Part 3: Iterating Over an Array with a 'for' loop ---\n");
    printf("All recorded temperatures:\n");

    // We will loop from index 0 up to (but not including) 5.
    // The loop counter `i` is used as the array index.
    for (int i = 0; i < 5; i++)
    {
        printf("  Day %d: %.1f\n", i + 1, temperatures[i]);
    }
    printf("\n");

    // Let's use a loop to calculate the sum of all high scores.
    int total_score = 0;
    for (int i = 0; i < 5; i++)
    {
        total_score = total_score + high_scores[i];
    }
    printf("Sum of all high scores: %d\n", total_score);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you've learned to manage collections of data.
 *
 * Key Takeaways:
 * - An ARRAY is a fixed-size collection of elements of the same type.
 * - C uses ZERO-BASED INDEXING, where the first element is at index 0.
 * - Accessing an array element is done with `array_name[index]`.
 * - You can initialize an array with a list of values like `int arr[] = {1, 2, 3};`.
 * - Loops, especially `for` loops, are essential for processing each element in an array.
 *
 * NOTE: For now, we manually kept track of the array's size (e.g., `i < 5`). In
 * future lessons, we will learn more robust ways to handle array sizes.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 8_arrays 8_arrays.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./8_arrays`
 *    - On Windows:       `8_arrays.exe`
 */