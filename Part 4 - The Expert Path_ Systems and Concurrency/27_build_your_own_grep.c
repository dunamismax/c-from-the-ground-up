/**
 * @file 27_build_your_own_grep.c
 * @brief Part 4, Lesson 27: Build Your Own grep
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file is a project that combines many of our previously learned skills
 * to build a simplified version of the famous `grep` command-line utility.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * PROJECT: BUILD YOUR OWN `grep`
 *
 * `grep` (which stands for Global Regular Expression Print) is one of the most
 * famous and widely used command-line utilities in the Unix world. Its job is
 * to search for a specific pattern of text inside files and print the lines
 * that contain a match.
 *
 * Our goal is to build a simplified version of `grep`. It won't support complex
 * regular expressions, but it will search for a fixed string within a file and
 * print any matching lines. This is a fantastic project because it combines:
 *
 * 1. COMMAND-LINE ARGUMENTS: To get the search pattern and the filename from the user.
 * 2. FILE I/O: To open and read the target file.
 * 3. STRING MANIPULATION: To search for the pattern within each line of the file.
 * 4. ERROR HANDLING: To manage cases where the user provides wrong input or the
 *    file doesn't exist.
 *
 * THE PLAN:
 * 1. Get two arguments from the command line: the search `pattern` and the `filename`.
 * 2. Open the `filename` for reading.
 * 3. Read the file one line at a time.
 * 4. For each line, check if it contains the `pattern`.
 * 5. If it does, print that line to the console.
 * 6. Close the file and exit.
 *
 * Let's get started!
 */

// --- Required Headers ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // We need this for strstr()

// The main function signature for programs that accept command-line arguments.
int main(int argc, char *argv[])
{
    // --- Step 1: Validate Command-Line Arguments ---

    // `argc` is the count of arguments. We expect exactly 3:
    // argv[0]: The program name (e.g., ./27_build_your_own_grep)
    // argv[1]: The search pattern (e.g., "main")
    // argv[2]: The filename (e.g., "27_build_your_own_grep.c")
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <pattern> <filename>\n", argv[0]);
        return 1; // Return an error code.
    }

    // Store the arguments in clearly named variables for readability.
    char *pattern = argv[1];
    char *filename = argv[2];

    // --- Step 2: Open the File ---

    // We declare a FILE POINTER. This pointer will hold the reference to our open file.
    FILE *file_pointer;

    // `fopen()` attempts to open the file.
    // The first argument is the path to the file.
    // The second argument, "r", specifies that we want to open it in READ mode.
    file_pointer = fopen(filename, "r");

    // If `fopen()` fails (e.g., the file doesn't exist), it returns NULL.
    // We must always check for this!
    if (file_pointer == NULL)
    {
        // `perror()` is a great function for printing file-related errors.
        // It prints your custom message, followed by a colon, and then the
        // system's human-readable error message for why the operation failed.
        perror("Error opening file");
        return 1;
    }

    // --- Step 3 & 4: Read File Line by Line and Search ---

    char line_buffer[2048]; // A buffer to hold one line of text from the file.

    printf("Searching for \"%s\" in file \"%s\":\n\n", pattern, filename);

    // This `while` loop is the standard, safe way to read a file line-by-line.
    // `fgets()` reads one line (or up to 2047 characters + null terminator)
    // from `file_pointer` and stores it in `line_buffer`.
    // It returns NULL when it reaches the end of the file, which ends the loop.
    while (fgets(line_buffer, sizeof(line_buffer), file_pointer) != NULL)
    {
        // --- The Core Logic: Searching for a Substring ---

        // `strstr()` is the key function here. It searches for the first
        // occurrence of the `pattern` string within the `line_buffer` string.
        // - If it finds a match, it returns a POINTER to the beginning of the match.
        // - If no match is found, it returns NULL.
        if (strstr(line_buffer, pattern) != NULL)
        {
            // A match was found! Print the entire line.
            // We use `printf("%s", ...)` and not `printf("%s\n", ...)` because
            // `fgets()` already includes the newline character `\n` at the
            // end of the line it reads.
            printf("%s", line_buffer);
        }
    }

    // --- Step 5: Clean Up ---

    // It is crucial to close the file when you are done with it.
    // `fclose()` releases the file handle back to the operating system.
    fclose(file_pointer);

    return 0; // Success!
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Congratulations! You've just built your own version of `grep`. This project is a
 * major milestone. It demonstrates your ability to write a complete, useful
 * command-line tool that interacts with the file system, processes user input,
 * and performs core logic using string manipulation.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal and compile the program:
 *    `gcc -Wall -Wextra -std=c11 -o 27_build_your_own_grep 27_build_your_own_grep.c`
 *
 * 2. Run it! A great first test is to make it search for something in its own source code.
 *    Let's search for every line containing the word "main":
 *    `./27_build_your_own_grep main 27_build_your_own_grep.c`
 *
 * 3. Try another one. Search for every line containing the `strstr` function:
 *    `./27_build_your_own_grep strstr 27_build_your_own_grep.c`
 *
 * 4. For a more realistic example, create a sample data file:
 *    `echo "Hello world, this is a test." > data.txt`
 *    `echo "Another line with some more data." >> data.txt`
 *    `echo "The world is full of interesting tests." >> data.txt`
 *    `echo "A final line without the keyword." >> data.txt`
 *
 *    Now search this new file for the word "world":
 *    `./27_build_your_own_grep world data.txt`
 *
 *    You should see the first and third lines printed to your console.
 */