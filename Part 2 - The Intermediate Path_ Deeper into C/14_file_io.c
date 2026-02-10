/**
 * @file 14_file_io.c
 * @brief Part 2, Lesson 14: File I/O
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for File I/O (Input/Output).
 * It explains how to write data to files and read data from them, allowing
 * programs to save and load information permanently.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * All our programs so far have had a temporary existence. When the program ends,
 * any data it was holding in variables, arrays, or structs is lost forever.
 *
 * To make our programs truly useful, we need PERSISTENCE—the ability to save
 * data to a storage device (like a hard drive) and load it back later. This is
 * achieved through FILE I/O (Input/Output).
 *
 * --- The Core Concepts of File I/O ---
 *
 * 1. The `FILE` Pointer:
 *    When you work with a file in C, you use a special pointer type called `FILE *`.
 *    This is a struct defined in `<stdio.h>` that holds all the information C
 *    needs to manage the connection to a file (its location, your current
 *    position in it, etc.). You never need to know what's inside the `FILE`
 *    struct; you just use a pointer to it.
 *
 * 2. `fopen()` - Opening a File:
 *    This function establishes a connection, or "stream," to a file on your disk.
 *    It takes two arguments: the filename and a "mode" string.
 *    - "w": WRITE mode. Creates a new file. If the file already exists, its
 *           contents are completely ERASED and overwritten.
 *    - "r": READ mode. Opens an existing file for reading. If the file doesn't
 *           exist, `fopen()` will fail.
 *    - "a": APPEND mode. Opens a file to add new data to the END of it. If the
 *           file doesn't exist, it will be created.
 *    `fopen()` returns a `FILE *` on success or `NULL` on failure. YOU MUST
 *    ALWAYS CHECK FOR `NULL`!
 *
 * 3. `fclose()` - Closing a File:
 *    This is the essential cleanup step. It finalizes any pending writes to the
 *    disk and tells the operating system you are done with the file, releasing
 *    resources. You MUST close every file you open.
 *
 * 4. Writing and Reading Functions:
 *    - `fprintf()`: Works just like `printf()`, but its first argument is a
 *      `FILE *`. It writes formatted text TO a file.
 *    - `fgets()`: Reads a line of text FROM a file. It's safer than other
 *      functions because you specify the maximum number of characters to read,
 *      preventing buffer overflows.
 */

#include <stdio.h>
#include <stdlib.h> // For exit()

int main(void)
{
    // A FILE pointer, always initialize to NULL.
    FILE *file_pointer = NULL;
    char *filename = "my_first_file.txt"; // The name of the file we will work with.

    // --- DEMONSTRATION 1: Writing to a File ---
    printf("--- Part 1: Writing to a file ---\n");

    // Step 1: Open the file in WRITE ("w") mode.
    file_pointer = fopen(filename, "w");

    // Step 2: ALWAYS check if the file opened successfully.
    if (file_pointer == NULL)
    {
        // stderr is the "standard error" stream, a good place for error messages.
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n", filename);
        exit(1); // exit() terminates the program immediately.
    }

    // Step 3: Write to the file using fprintf().
    printf("Writing data to %s...\n", filename);
    fprintf(file_pointer, "Hello, File!\n");
    fprintf(file_pointer, "This is the second line.\n");
    fprintf(file_pointer, "Player: %s, Score: %d\n", "Max", 100);

    // Step 4: Close the file.
    fclose(file_pointer);
    file_pointer = NULL; // Good practice to nullify the pointer after closing.
    printf("Successfully wrote to and closed the file.\n\n");

    // --- DEMONSTRATION 2: Reading from a File ---
    printf("--- Part 2: Reading from a file ---\n");

    // This buffer will hold the lines we read from the file.
    char line_buffer[256];

    // Step 1: Open the same file, but now in READ ("r") mode.
    file_pointer = fopen(filename, "r");

    // Step 2: Check for errors again. The file might have been deleted, or
    // we may not have permission to read it.
    if (file_pointer == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s' for reading.\n", filename);
        exit(1);
    }

    // Step 3: Read from the file line-by-line using fgets().
    // `fgets()` reads until it hits a newline, the end of the file, or the
    // buffer size limit is reached. It returns NULL when there is nothing left to read.
    printf("Reading data from %s:\n", filename);
    while (fgets(line_buffer, sizeof(line_buffer), file_pointer) != NULL)
    {
        // Print the line we just read from the file to the console.
        printf("%s", line_buffer);
    }

    // Step 4: Close the file.
    fclose(file_pointer);
    printf("\nSuccessfully read and closed the file.\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  File I/O allows your program's data to be PERSISTENT.
 * 2.  The `FILE *` is your handle to an open file.
 * 3.  The basic workflow is always: OPEN -> VALIDATE -> READ/WRITE -> CLOSE.
 * 4.  `fopen()` opens a file with a specific mode (`"w"`, `"r"`, `"a"`). You MUST
 *     check its return value for `NULL` to handle errors.
 * 5.  `fprintf()` is like `printf()` but writes to a file stream.
 * 6.  `fgets()` is a safe and reliable way to read text line-by-line from a file.
 * 7.  `fclose()` is mandatory to save all changes and release system resources.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 14_file_io 14_file_io.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./14_file_io`
 *    - On Windows:       `14_file_io.exe`
 *
 * After running, check the directory! You will find a new file named
 * `my_first_file.txt` containing the text from the program.
 */