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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Global Constants and Type Definitions ---
#define MAX_STUDENTS 100
#define MAX_NAME_LEN 50
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
void clear_input_buffer(void);

// --- Main Function: The Program's Control Center ---
int main(void)
{
    Student all_students[MAX_STUDENTS];
    int student_count = 0;
    int choice = 0;

    // Start by loading any existing records from our database file.
    load_from_file(all_students, &student_count);

    // This is the main application loop. It continues until the user chooses to exit.
    while (1)
    {
        display_menu();
        // Read the user's menu choice.
        if (scanf("%d", &choice) != 1)
        {
            // If input is not a number, handle the error.
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue; // Skip the rest of the loop and start over.
        }
        clear_input_buffer(); // Clean up leftover newline from scanf.

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

    return 0; // This line is technically unreachable but good practice.
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
 * @brief Clears the standard input buffer to prevent issues with scanf.
 */
void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        // Consume characters until newline or end-of-file.
    }
}

/**
 * @brief Adds a new student record to the array.
 * @param students The array of student records.
 * @param count A pointer to the current number of students. We use a pointer
 *              so we can modify the original `student_count` in `main`.
 */
void add_student(Student students[], int *count)
{
    if (*count >= MAX_STUDENTS)
    {
        printf("Database is full. Cannot add more students.\n");
        return;
    }

    printf("Enter Student ID: ");
    scanf("%d", &students[*count].id);
    clear_input_buffer();

    printf("Enter Student Name: ");
    // Read a full line, including spaces, up to the size limit.
    fgets(students[*count].name, MAX_NAME_LEN, stdin);
    // fgets includes the newline character, so we remove it.
    students[*count].name[strcspn(students[*count].name, "\n")] = 0;

    printf("Enter Student GPA: ");
    scanf("%lf", &students[*count].gpa);
    clear_input_buffer();

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
        // We save in a comma-separated format. This is a common, simple format.
        fprintf(file, "%d,%s,%.2f\n", students[i].id, students[i].name, students[i].gpa);
    }

    fclose(file);
    printf("Successfully saved %d record(s) to %s.\n", count, FILENAME);
}

/**
 * @brief Loads student records from a file into the array.
 */
void load_from_file(Student students[], int *count)
{
    FILE *file = fopen(FILENAME, "r"); // "r" for read.
    if (file == NULL)
    {
        // This is not an error if it's the first time running the program.
        printf("No existing database file found. Starting fresh.\n");
        return;
    }

    // `fscanf` returns the number of items successfully read. We expect 3.
    // The format string `[^,]` reads all characters until a comma is found.
    while (*count < MAX_STUDENTS &&
           fscanf(file, "%d,%49[^,],%lf\n", &students[*count].id, students[*count].name, &students[*count].gpa) == 3)
    {
        (*count)++;
    }

    fclose(file);
    printf("Successfully loaded %d record(s) from %s.\n", *count, FILENAME);
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
 *    `gcc -Wall -Wextra -std=c11 -o 17_student_record_system 17_student_record_system.c`
 *
 * 4. Run the executable:
 *    - On Linux/macOS:   `./17_student_record_system`
 *    - On Windows:       `17_student_record_system.exe`
 *
 * Try adding a few students, saving, exiting the program, and running it again.
 * You'll see your records are loaded back in automatically!
 */