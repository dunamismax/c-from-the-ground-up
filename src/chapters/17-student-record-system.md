# Student Record System

> **What you'll learn:** putting functions, structs, file I/O and input checking together in a menu-driven program that saves its records.
>
> **Before you start:** Lessons 12-15.

This project is where everything comes together. We will build a complete
application that can:
1. Add new student records.
2. Display all existing records.
3. Save the records to a file so the data isn't lost when the program closes.
4. Load the records from the file when the program starts.

This is a foundational pattern for almost any data-management application.

## Key Architecture Concepts

- MODULARITY: We will break the program into small, single-purpose functions
  (e.g., `add_student`, `save_to_file`). This makes the code much easier to
  read, debug, and maintain. The `main` function will act as a control center.
- DATA PERSISTENCE: By using File I/O, our application's data will persist
  between runs.
- USER INTERFACE: We'll create a simple text-based menu to interact with the user.
- ERROR HANDLING: The program will gracefully handle file errors and invalid
  user input.

Functions that change how many records there are, such as `add_student`,
take a pointer to the count (`int *count`), so they can modify the original
`student_count` in `main`. Functions that only read the records, such as
`print_all_records`, take them as `const`: a promise that the function will
not change the data.

## Full Source

<!-- source: Part 3 - The Advanced Path_ Towards Mastery/17_student_record_system.c -->
```c
/**
 * @file 17_student_record_system.c
 * @brief Part 3, Project 17: Student Record System
 * @author dunamismax
 * @date 06-15-2025
 *
 * This project builds a menu-driven database application to manage student
 * records. It demonstrates the power of combining structs, arrays, functions,
 * and file I/O to create a persistent data management tool.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * This project is where everything comes together. We will build a complete
 * application that can:
 * 1. Add new student records.
 * 2. Display all existing records.
 * 3. Save the records to a file so the data isn't lost when the program closes.
 * 4. Load the records from the file when the program starts.
 *
 * This is a foundational pattern for almost any data-management application.
 *
 * KEY ARCHITECTURE CONCEPTS:
 * - MODULARITY: We will break the program into small, single-purpose functions
 *   (e.g., `add_student`, `save_to_file`). This makes the code much easier to
 *   read, debug, and maintain. The `main` function will act as a control center.
 * - DATA PERSISTENCE: By using File I/O, our application's data will persist
 *   between runs.
 * - USER INTERFACE: We'll create a simple text-based menu to interact with the user.
 * - ERROR HANDLING: The program will gracefully handle file errors and invalid
 *   user input.
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Global Constants and Type Definitions ---
#define MAX_STUDENTS 100
#define MAX_NAME_LEN 50
#define INPUT_BUFFER_SIZE 128
#define FILENAME "students.db"

// Our main data structure for a single student.
typedef struct
{
    int id;
    char name[MAX_NAME_LEN];
    double gpa;
} Student;

// --- Function Prototypes ---
// Declaring all our functions here provides a nice overview of the program's
// capabilities and allows us to call them from `main` before they are defined.
void display_menu(void);
void add_student(Student students[], int *count);
void print_all_records(const Student students[], int count);
void save_to_file(const Student students[], int count);
void load_from_file(Student students[], int *count);
void clear_stream_remainder(FILE *stream);
void clear_input_buffer(void);
int read_line_from_stream(FILE *stream, char *buffer, size_t size);
int read_line(char *buffer, size_t size);
int parse_int_value(const char *text, int *value);
int parse_double_value(const char *text, double *value);
int is_blank_string(const char *text);
int parse_student_record(char *line, Student *student);

// --- Main Function: The Program's Control Center ---
int main(void)
{
    Student all_students[MAX_STUDENTS];
    int student_count = 0;
    int choice = 0;
    char input[INPUT_BUFFER_SIZE];

    // Start by loading any existing records from our database file.
    load_from_file(all_students, &student_count);

    // This is the main application loop. It continues until the user chooses to exit.
    while (1)
    {
        display_menu();
        if (read_line(input, sizeof(input)) == 0)
        {
            printf("\nEnd of input detected. Exiting program.\n");
            break;
        }

        if (!parse_int_value(input, &choice))
        {
            printf("Invalid input. Please enter a number.\n");
            continue; // Skip the rest of the loop and start over.
        }

        switch (choice)
        {
        case 1:
            add_student(all_students, &student_count);
            break;
        case 2:
            print_all_records(all_students, student_count);
            break;
        case 3:
            save_to_file(all_students, student_count);
            break;
        case 4:
            printf("Exiting program. Goodbye!\n");
            exit(0); // exit(0) terminates the program successfully.
        default:
            printf("Invalid choice. Please try again.\n");
        }
        printf("\n"); // Add a space for readability before the next menu.
    }

    return 0;
}

// --- Function Implementations ---

/**
 * @brief Displays the main menu options to the user.
 */
void display_menu(void)
{
    printf("--- Student Record System ---\n");
    printf("1. Add Student\n");
    printf("2. Display All Records\n");
    printf("3. Save Records to File\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
}

/**
 * @brief Consumes characters until the end of the current line.
 */
void clear_stream_remainder(FILE *stream)
{
    int c;
    while ((c = fgetc(stream)) != '\n' && c != EOF)
    {
        // Consume characters until newline or end-of-file.
    }
}

void clear_input_buffer(void)
{
    clear_stream_remainder(stdin);
}

/**
 * @brief Reads a full line from the provided stream into a buffer.
 * @return 1 on success, 0 on EOF/error, and -1 if the line was too long.
 */
int read_line_from_stream(FILE *stream, char *buffer, size_t size)
{
    size_t length;

    // `fgets` takes its size as an `int`, while sizes are `size_t`. Our
    // buffers hold at most 256 bytes, so the cast cannot lose anything.
    if (fgets(buffer, (int)size, stream) == NULL)
    {
        return 0;
    }

    length = strcspn(buffer, "\n");
    if (buffer[length] == '\n')
    {
        buffer[length] = '\0';
        return 1;
    }

    clear_stream_remainder(stream);
    buffer[0] = '\0';
    return -1;
}

int read_line(char *buffer, size_t size)
{
    return read_line_from_stream(stdin, buffer, size);
}

/**
 * @brief Parses a whole-number input and rejects trailing junk.
 */
int parse_int_value(const char *text, int *value)
{
    char *endptr;
    long parsed;

    while (isspace((unsigned char)*text))
    {
        text++;
    }

    if (*text == '\0')
    {
        return 0;
    }

    errno = 0;
    parsed = strtol(text, &endptr, 10);
    if (errno != 0 || endptr == text || parsed < INT_MIN || parsed > INT_MAX)
    {
        return 0;
    }

    while (isspace((unsigned char)*endptr))
    {
        endptr++;
    }

    if (*endptr != '\0')
    {
        return 0;
    }

    *value = (int)parsed;
    return 1;
}

/**
 * @brief Parses a floating-point value and rejects trailing junk.
 */
int parse_double_value(const char *text, double *value)
{
    char *endptr;
    double parsed;

    while (isspace((unsigned char)*text))
    {
        text++;
    }

    if (*text == '\0')
    {
        return 0;
    }

    errno = 0;
    parsed = strtod(text, &endptr);
    if (errno != 0 || endptr == text)
    {
        return 0;
    }

    while (isspace((unsigned char)*endptr))
    {
        endptr++;
    }

    if (*endptr != '\0')
    {
        return 0;
    }

    *value = parsed;
    return 1;
}

int is_blank_string(const char *text)
{
    while (*text)
    {
        if (!isspace((unsigned char)*text))
        {
            return 0;
        }
        text++;
    }

    return 1;
}

/**
 * @brief Parses one database record from the on-disk format.
 */
int parse_student_record(char *line, Student *student)
{
    char *first_comma = strchr(line, ',');
    char *second_comma;

    if (!first_comma)
    {
        return 0;
    }

    second_comma = strchr(first_comma + 1, ',');
    if (!second_comma || strchr(second_comma + 1, ',') != NULL)
    {
        return 0;
    }

    *first_comma = '\0';
    *second_comma = '\0';

    if (!parse_int_value(line, &student->id) || student->id <= 0)
    {
        return 0;
    }

    if (strlen(first_comma + 1) >= MAX_NAME_LEN ||
        strchr(first_comma + 1, ',') != NULL ||
        is_blank_string(first_comma + 1))
    {
        return 0;
    }
    // `strcpy` does not check sizes, but this copy is safe: the check above
    // rejected any name of MAX_NAME_LEN characters or more, so the name and
    // its `\0` always fit in `student->name`.
    strcpy(student->name, first_comma + 1);

    if (!parse_double_value(second_comma + 1, &student->gpa) ||
        student->gpa < 0.0 || student->gpa > 4.0)
    {
        return 0;
    }

    return 1;
}

/**
 * @brief Adds a new student record to the array.
 * @param students The array of student records.
 * @param count A pointer to the current number of students. We use a pointer
 *              so we can modify the original `student_count` in `main`.
 */
void add_student(Student students[], int *count)
{
    Student new_student;
    char input[INPUT_BUFFER_SIZE];

    if (*count >= MAX_STUDENTS)
    {
        printf("Database is full. Cannot add more students.\n");
        return;
    }

    printf("Enter Student ID: ");
    if (read_line(input, sizeof(input)) != 1 ||
        !parse_int_value(input, &new_student.id) ||
        new_student.id <= 0)
    {
        printf("Invalid student ID. Record was not added.\n");
        return;
    }

    printf("Enter Student Name (commas are not allowed): ");
    if (read_line(new_student.name, sizeof(new_student.name)) != 1)
    {
        printf("Name was too long or could not be read. Record was not added.\n");
        return;
    }

    if (strchr(new_student.name, ',') != NULL)
    {
        printf("Names cannot contain commas because the save file uses commas as separators.\n");
        return;
    }

    if (is_blank_string(new_student.name))
    {
        printf("Student name cannot be empty. Record was not added.\n");
        return;
    }

    printf("Enter Student GPA: ");
    if (read_line(input, sizeof(input)) != 1 ||
        !parse_double_value(input, &new_student.gpa) ||
        new_student.gpa < 0.0 || new_student.gpa > 4.0)
    {
        printf("Invalid GPA. Enter a value between 0.0 and 4.0. Record was not added.\n");
        return;
    }

    students[*count] = new_student;
    printf("Student added successfully.\n");
    (*count)++; // Increment the total number of students.
}

/**
 * @brief Prints all student records to the console in a formatted table.
 * @param students The array of student records.
 * @param count The current number of students.
 *              `const` is used to signal that this function will not change the data.
 */
void print_all_records(const Student students[], int count)
{
    if (count == 0)
    {
        printf("No records to display.\n");
        return;
    }

    printf("\n--- All Student Records ---\n");
    printf("ID   | Name                                               | GPA\n");
    printf("-----|----------------------------------------------------|------\n");
    for (int i = 0; i < count; i++)
    {
        // %-4d: left-align integer in 4 spaces
        // %-50s: left-align string in 50 spaces
        // %5.2f: right-align double in 5 spaces, with 2 decimal places
        printf("%-4d | %-50s | %5.2f\n", students[i].id, students[i].name, students[i].gpa);
    }
    printf("------------------------------------------------------------------\n");
}

/**
 * @brief Saves the entire array of student records to a file.
 */
void save_to_file(const Student students[], int count)
{
    FILE *file = fopen(FILENAME, "w"); // "w" for write, will overwrite existing file.
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n", FILENAME);
        return;
    }

    for (int i = 0; i < count; i++)
    {
        // We save in a simple comma-separated format, so names must not contain commas.
        fprintf(file, "%d,%s,%.2f\n", students[i].id, students[i].name, students[i].gpa);
    }

    // Writes are buffered, so a full disk may only show up when `fclose` writes
    // the last of them. `ferror` reports whether any earlier write failed.
    // Only claim success when every write and the close worked.
    int write_failed = ferror(file);
    if (fclose(file) != 0 || write_failed)
    {
        fprintf(stderr, "Error: Could not finish writing '%s'. Records may not be saved.\n", FILENAME);
        return;
    }
    printf("Successfully saved %d record(s) to %s.\n", count, FILENAME);
}

/**
 * @brief Loads student records from a file into the array.
 */
void load_from_file(Student students[], int *count)
{
    FILE *file = fopen(FILENAME, "r"); // "r" for read.
    char line[INPUT_BUFFER_SIZE * 2];
    int loaded = 0;
    int skipped = 0;
    int line_status;

    if (file == NULL)
    {
        // This is not an error if it's the first time running the program.
        printf("No existing database file found. Starting fresh.\n");
        return;
    }

    while ((line_status = read_line_from_stream(file, line, sizeof(line))) != 0)
    {
        if (line_status < 0)
        {
            skipped++;
            continue;
        }

        if (line[0] == '\0')
        {
            continue;
        }

        if (*count >= MAX_STUDENTS)
        {
            skipped++;
            continue;
        }

        if (parse_student_record(line, &students[*count]))
        {
            (*count)++;
            loaded++;
        }
        else
        {
            skipped++;
        }
    }

    fclose(file);
    printf("Successfully loaded %d record(s) from %s.\n", loaded, FILENAME);
    if (skipped > 0)
    {
        printf("Skipped %d invalid record(s) while loading.\n", skipped);
    }
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Congratulations! You've built a complete database application. This project is a
 * major milestone and demonstrates a solid understanding of C's most important
 * features for building practical software.
 *
 * Key Project Achievements:
 * - A modular design using functions for each major feature.
 * - Persistent data storage using a simple comma-separated file format.
 * - A clean, interactive user menu for program control.
 * - Robust handling of user input and file system operations.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c17 -o 17_student_record_system 17_student_record_system.c`
 *
 * 4. Run the executable:
 *    - On Linux/macOS:   `./17_student_record_system`
 *    - On Windows (WSL): `./17_student_record_system`
 *
 * Try adding a few students, saving, exiting the program, and running it again.
 * You'll see your records are loaded back in automatically!
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c17 -o 17_student_record_system 17_student_record_system.c
./17_student_record_system
```

## Try It Yourself

Practise what this lesson taught. Change a copy of the lesson file, or
start a new one.

1. Add a menu option that finds a student by ID and prints their record.
2. Add a menu option that prints the average GPA of all students.
3. Open the saved file in a text editor, make one line malformed, and see how loading handles it.
