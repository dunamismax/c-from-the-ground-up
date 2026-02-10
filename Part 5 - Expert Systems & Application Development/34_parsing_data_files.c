/**
 * @file 34_parsing_data_files.c
 * @brief Part 5, Lesson 34: Parsing Structured Data Files
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for reading and parsing
 * structured data files, like CSVs or configuration files.
 * It explains the core concepts through structured comments and
 * provides a runnable example of their implementation.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, our programs have either been non-persistent or have saved data in a
 * simple, unstructured way. Real-world applications, however, almost always
 * need to read structured data—user profiles, game levels, configuration
 * settings, etc. The process of reading this data and converting it into a
 * useful format (like a `struct`) is called PARSING.
 *
 * Today, we will learn how to parse a simple Comma-Separated Values (CSV) file.
 * Each line in the file will represent a single record, with fields separated
 * by commas.
 *
 * THE GOAL:
 * We will write a program that reads a file named `users.dat`. Each line of
 * this file will contain a user's ID, username, level, and active status.
 * Our program will parse each line and load the data into an array of `struct User`.
 *
 * We will explore two primary methods for parsing strings: `strtok` and `sscanf`.
 * As you will see, `sscanf` is often safer and more robust.
 */

#include <stdio.h>
#include <stdlib.h> // For atoi() in the strtok example
#include <string.h> // For strtok(), strcpy()

#define MAX_LINE_LENGTH 256
#define MAX_USERS 10

// --- Part 1: The Data Structure ---
// First, we need a struct to hold the data for a single user.
// This structure directly mirrors the format of a line in our data file.
typedef struct
{
    int id;
    char username[50];
    int level;
    int is_active; // 1 for active, 0 for inactive
} User;

// --- Function Prototypes ---
void parse_with_sscanf(FILE *file);
void print_user(const User *user);

// --- The Main Function ---
// Our program will expect the name of the data file as a command-line argument.
int main(int argc, char *argv[])
{
    // A program that needs a file to work should always check for it.
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <datafile>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        // `perror` is a useful function from <stdio.h> that prints our message
        // followed by a system error message for why the operation failed.
        perror("Error opening file");
        return 1;
    }

    printf("--- Parsing data file using sscanf() ---\n");
    parse_with_sscanf(file);

    fclose(file);
    return 0;
}

// --- Part 2: Parsing with sscanf() ---
/*
 * `sscanf` is often the best tool for this job. It's like `scanf`, but it reads
 * from a string instead of the keyboard. It's powerful because it can parse
 * complex patterns, and its return value tells you how many items were
 * successfully assigned, which makes error checking very easy.
 */
void parse_with_sscanf(FILE *file)
{
    char line[MAX_LINE_LENGTH];
    User users[MAX_USERS];
    int user_count = 0;

    // We read the file line by line using `fgets`. This is safe because it
    // prevents buffer overflows by never reading more than `MAX_LINE_LENGTH`
    // characters at a time.
    while (fgets(line, sizeof(line), file) != NULL && user_count < MAX_USERS)
    {
        User current_user;

        // This is the core of the sscanf method. Let's break down the format string:
        // "%d"       - Reads an integer (the ID).
        // ","        - Matches and discards a literal comma.
        // "%49[^,]"  - This is the clever part for reading the username.
        //   `%[...]` is a scanset. It reads characters as long as they are in the set.
        //   `^` negates the set, so `[^,]` reads characters as long as they are NOT a comma.
        //   `49` is a width limit to prevent buffer overflow on `username[50]`.
        // ",%d,%d"   - Reads comma, integer, comma, integer for level and status.
        //
        // CRITICAL: We pass POINTERS to the variables where sscanf should store
        // the data. For integers, we use the address-of operator `&`. For the
        // character array `username`, the name itself decays to a pointer.
        int items_scanned = sscanf(line, "%d,%49[^,],%d,%d",
                                   current_user.id,
                                   current_user.username,
                                   current_user.level,
                                   current_user.is_active);

        // We expect to scan 4 items. If we don't, the line was malformed.
        if (items_scanned == 4)
        {
            users[user_count] = current_user;
            user_count++;
        }
        else
        {
            fprintf(stderr, "Warning: Malformed line skipped: %s", line);
        }
    }

    // Now, let's print out the data we successfully parsed.
    printf("\nSuccessfully parsed %d user records:\n", user_count);
    for (int i = 0; i < user_count; i++)
    {
        print_user(&users[i]);
    }
}

/**
 * @brief A helper function to print the contents of a User struct.
 * @param user A pointer to the User struct to be printed.
 */
void print_user(const User *user)
{
    // The formatting specifiers `%-5d` and `%-15s` left-align the output
    // in a field of a certain width, creating nice columns.
    printf("  User ID: %-5d | Username: %-15s | Level: %-3d | Active: %s\n",
           user->id,
           user->username,
           user->level,
           user->is_active ? "Yes" : "No"); // Using a ternary operator for clean output.
}

/*
 * =====================================================================================
 * |                    - (For Reference) Parsing with strtok() -                      |
 * =====================================================================================
 *
 * `strtok` is another function for splitting a string. It works by finding a
 * delimiter, replacing it with a null terminator (`\0`), and returning a pointer
 * to the start of the token.
 *
 * WHY IS IT NOT PREFERRED?
 * 1. It MODIFIES the original string. This can be destructive and surprising.
 * 2. It is STATEFUL. You call it once with the string, then with NULL for
 *    subsequent tokens. This makes the code more complex and means you can't
 *    use it to parse two strings at once (it is not "re-entrant" or thread-safe).
 *
 * While `sscanf` is generally better, seeing a `strtok` implementation is useful
 * as you will encounter it in existing C code.
 *
 * Example `strtok` loop:
 *
 *   // Important: `strtok` modifies the string, so we'd need a copy.
 *   // char line_copy[MAX_LINE_LENGTH];
 *   // strcpy(line_copy, line);
 *
 *   char *token = strtok(line, ","); // Get first token
 *   if (token) user.id = atoi(token);
 *
 *   token = strtok(NULL, ","); // Get next token
 *   if (token) strcpy(user.username, token);
 *
 *   ... and so on. As you can see, it's more verbose and requires more care.
 */

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * This program requires a data file to read. You must create it first.
 *
 * 1. CREATE THE DATA FILE:
 *    In the same directory as this C file, create a new file named `users.dat`.
 *    Copy and paste the following content into it:
 *
 *    101,alpha_coder,15,1
 *    102,bit_wizard,22,1
 *    205,kernel_hacker,45,0
 *    this is a bad line and should be skipped
 *    310,logic_lord,33,1
 *    404,syntax_sorcerer,99,1
 *
 * 2. COMPILE THE C FILE:
 *    Open a terminal and navigate to the directory.
 *    `gcc -Wall -Wextra -std=c11 -o 34_parsing_data_files 34_parsing_data_files.c`
 *
 * 3. RUN THE EXECUTABLE:
 *    You must provide the name of the data file as a command-line argument.
 *    - On Linux/macOS:   `./34_parsing_data_files users.dat`
 *    - On Windows:       `34_parsing_data_files.exe users.dat`
 *
 *    The program will read `users.dat`, parse its contents, print a warning for
 *    the bad line, and then display the structured data it successfully extracted.
 */