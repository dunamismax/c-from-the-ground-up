/**
 * @file 9_strings.c
 * @brief Part 1, Lesson 9: Strings
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson covers how to work with strings, which are one of the most
 * common types of data in programming.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Up to this point, we've only seen string "literals" (text in double quotes)
 * used directly in `printf`. We haven't stored or manipulated them.
 *
 * THE BIG IDEA: C does not have a built-in `string` data type like many other
 * languages. Instead, a C STRING is simply an ARRAY of characters (`char`).
 *
 * But how does the computer know where the string ends in the array?
 *
 * THE NULL TERMINATOR
 * C strings follow a crucial convention: they are "null-terminated". This means
 * a special character, the NULL TERMINATOR `\0`, is placed after the last
 * actual character in the array to mark the end of the string.
 *
 * So, the string "Hello" is stored in a `char` array as:
 * ['H', 'e', 'l', 'l', 'o', '\0']
 * It requires 6 characters of storage, not 5! This is a very important concept.
 *
 * All standard C functions that work with strings rely on this `\0` character.
 */

#include <stdio.h>
#include <string.h> // We must include this new header for string manipulation functions.

int main(void)
{
    // --- Part 1: Declaring and Printing Strings ---

    /*
     * The easiest way to declare and initialize a string is to use a string
     * literal. The compiler automatically adds the null terminator `\0` for you.
     */
    char greeting[] = "Hello, C programmer!";

    printf("--- Part 1: Declaring and Printing Strings ---\n");
    /*
     * The `%s` format specifier in `printf` is used for strings. When `printf`
     * receives a `char` array with `%s`, it prints characters one by one
     * until it finds the null terminator `\0`.
     */
    printf("Our greeting string: %s\n", greeting);

    // Let's prove the null terminator exists. We'll manually initialize one.
    // This is the "hard way" but illustrates the concept perfectly.
    char name[] = {'A', 'l', 'i', 'c', 'e', '\0'};
    printf("A manually created name: %s\n\n", name);

    // --- Part 2: The `<string.h>` Library ---

    /*
     * Because strings are so common, C provides a standard library of functions
     * to work with them. We included `<string.h>` at the top to get access.
     */
    printf("--- Part 2: String Functions ---\n");

    // `strlen()` - String Length
    // This function counts the number of characters in a string *before* the
    // null terminator.
    char message[] = "This is a test.";
    // The length is 15, even though the array size is 16 (to hold the `\0`).
    printf("The message is: \"%s\"\n", message);
    printf("The length of the message (strlen) is: %zu\n", strlen(message));
    // Note: strlen returns a special unsigned integer type `size_t`. We use
    // the `%zu` format specifier to print it correctly and avoid warnings.
    printf("\n");

    // --- Part 3: Copying and Concatenating Strings ---

    printf("--- Part 3: Copying and Concatenating ---\n");

    /*
     * `strcpy()` - String Copy
     * You CANNOT assign one string to another using `=`.
     * `char another_string[20]; another_string = message;` <-- THIS IS WRONG!
     *
     * You must copy the characters from the source to the destination.
     * WARNING: The destination array must be large enough to hold the source
     * string, including its null terminator!
     */
    char destination[50]; // A buffer to copy into.
    char source[] = "Some text to be copied.";

    strcpy(destination, source); // Copies `source` into `destination`.
    printf("Result of strcpy: %s\n", destination);

    /*
     * `strcat()` - String Concatenate (join)
     * This function appends one string to the end of another.
     * WARNING: Again, the destination string must have enough space for the
     * combined result!
     */
    char full_greeting[50] = "Welcome, "; // Initialize with enough space.
    char user[] = "dunamismax";

    strcat(full_greeting, user); // Appends `user` to `full_greeting`.
    printf("Result of strcat: %s\n\n", full_greeting);

    // --- Part 4: Comparing Strings ---

    printf("--- Part 4: Comparing Strings ---\n");

    /*
     * `strcmp()` - String Compare
     * You CANNOT compare strings using `==`.
     * `if (str1 == str2)` <-- THIS IS WRONG! It compares memory locations, not content.
     *
     * `strcmp()` compares two strings lexicographically (like in a dictionary).
     * It returns:
     *   - 0 if the strings are identical.
     *   - A negative value if the first string comes before the second.
     *   - A positive value if the first string comes after the second.
     */
    char pass1[] = "password123";
    char pass2[] = "Password123"; // Case matters!
    char pass3[] = "password123";

    if (strcmp(pass1, pass2) == 0)
    {
        printf("\"%s\" and \"%s\" are the same.\n", pass1, pass2);
    }
    else
    {
        printf("\"%s\" and \"%s\" are different.\n", pass1, pass2);
    }

    if (strcmp(pass1, pass3) == 0)
    {
        printf("\"%s\" and \"%s\" are the same.\n", pass1, pass3);
    }

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Congratulations on completing Part 1! Strings are a huge concept.
 *
 * Key Takeaways:
 * - A C string is a `char` array ending with a null terminator `\0`.
 * - The `%s` format specifier is used to print strings.
 * - The `<string.h>` header provides essential functions for strings.
 * - `strlen()` gets the length (not including `\0`).
 * - `strcpy()` copies one string to another.
 * - `strcat()` joins one string to the end of another.
 * - `strcmp()` compares the content of two strings.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 9_strings 9_strings.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./9_strings`
 *    - On Windows:       `9_strings.exe`
 */