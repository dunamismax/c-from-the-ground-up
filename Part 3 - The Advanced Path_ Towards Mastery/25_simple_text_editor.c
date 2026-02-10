/**
 * @file 25_simple_text_editor.c
 * @brief Part 3, Capstone Project: Simple Text Editor
 * @author dunamismax
 * @date 06-15-2025
 *
 * This capstone project builds a functional, line-based text editor.
 * It integrates many concepts from the course: doubly-linked lists, dynamic
 * memory allocation, file I/O, command-line arguments, and string manipulation.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Welcome to your Part 3 capstone project! This is where we put everything
 * together to build a substantial application: a command-line text editor.
 *
 * Our editor, which we'll call `sled` (Simple Line Editor), will be able to
 * open a file, display its contents, add/delete lines, and save the changes.
 *
 * THE CORE DATA STRUCTURE:
 * The heart of our editor is a DOUBLY-LINKED LIST. This is an enhancement of the
 * linked list from lesson 20. In a doubly-linked list, each node points not
 * only to the NEXT node but also to the PREVIOUS one. This makes operations like
 * inserting and deleting in the middle of the list much more efficient.
 *
 * Each node in our list will represent one line of text in the file.
 *
 * SKILLS INTEGRATED:
 * - Structs: To define the `Line` node for our list.
 * - Pointers: Extensively used for linking nodes (`next`, `prev`) and managing data.
 * - Dynamic Memory (`malloc`/`free`): To create and destroy lines as needed.
 * - File I/O (`fopen`, `fgets`, `fprintf`): To load and save the file.
 * - Command-Line Arguments (`argc`, `argv`): To specify which file to edit.
 * - Functions: To create a clean, modular, and readable program structure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // For a true/false `modified` flag

// --- Data Structures and Global State ---

// The node for our doubly-linked list. Each node is one line of text.
typedef struct Line
{
    char *text;        // Dynamically allocated text of the line
    struct Line *prev; // Pointer to the previous line
    struct Line *next; // Pointer to the next line
} Line;

// Global pointers to the start and end of our list (the "text buffer")
Line *head = NULL;
Line *tail = NULL;
int line_count = 0;
bool modified = false; // Flag to track if the file has been changed

// --- Function Prototypes ---
void load_file(const char *filename);
void save_file(const char *filename);
void free_buffer(void);
void print_lines(void);
void insert_line(int line_number, const char *text);
void delete_line(int line_number);
void append_line(const char *text);
void print_help(void);

// --- Main Function: The Editor's Control Loop ---

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    load_file(filename);

    char input_buffer[1024];
    printf("Simple Line Editor. Type 'h' for help, 'q' to quit.\n");

    while (true)
    {
        printf("> ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL)
        {
            break; // Exit on EOF (Ctrl+D)
        }

        // Parse command and arguments
        char command = input_buffer[0];
        char *argument = input_buffer + 2;
        int line_num = atoi(argument);

        switch (command)
        {
        case 'p':
            print_lines();
            break;
        case 'a': // Append
            // Remove newline from argument
            argument[strcspn(argument, "\n")] = 0;
            append_line(argument);
            break;
        case 'i': // Insert
        {
            char *text_start = strchr(argument, ' ');
            if (text_start)
            {
                *text_start = '\0'; // Split line number from text
                text_start++;
                text_start[strcspn(text_start, "\n")] = 0;
                insert_line(atoi(argument), text_start);
            }
            else
            {
                printf("Usage: i <line_number> <text>\n");
            }
            break;
        }
        case 'd': // Delete
            delete_line(line_num);
            break;
        case 's':
            save_file(filename);
            break;
        case 'h':
            print_help();
            break;
        case 'q':
            if (modified)
            {
                printf("File has unsaved changes. Save first? (y/n): ");
                if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL &&
                    (input_buffer[0] == 'y' || input_buffer[0] == 'Y'))
                {
                    save_file(filename);
                }
            }
            printf("Exiting.\n");
            free_buffer();
            return 0;
        default:
            printf("Unknown command. Type 'h' for help.\n");
        }
    }

    free_buffer();
    return 0;
}

// --- Function Implementations ---

/**
 * @brief Prints the help menu.
 */
void print_help(void)
{
    printf("--- sled Help ---\n");
    printf("p              - Print all lines\n");
    printf("a <text>       - Append a new line of text to the end\n");
    printf("i <num> <text> - Insert text before line <num>\n");
    printf("d <num>        - Delete line <num>\n");
    printf("s              - Save the file\n");
    printf("h              - Show this help message\n");
    printf("q              - Quit the editor\n");
}

/**
 * @brief Allocates a new line node.
 */
Line *create_line(const char *text)
{
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (!new_line)
    {
        perror("malloc failed for new line");
        exit(1);
    }
    // Allocate space for the text and copy it over.
    new_line->text = malloc(strlen(text) + 1);
    if (!new_line->text)
    {
        perror("malloc failed for line text");
        exit(1);
    }
    strcpy(new_line->text, text);
    new_line->prev = NULL;
    new_line->next = NULL;
    return new_line;
}

/**
 * @brief Appends a new line to the end of the text buffer.
 */
void append_line(const char *text)
{
    Line *new_line = create_line(text);
    if (tail == NULL)
    { // List is empty
        head = tail = new_line;
    }
    else
    {
        tail->next = new_line;
        new_line->prev = tail;
        tail = new_line;
    }
    line_count++;
    modified = true;
}

/**
 * @brief Loads the content of a file into the linked-list buffer.
 */
void load_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("New file: '%s'\n", filename);
        return; // It's okay if the file doesn't exist yet
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline
        append_line(buffer);
    }
    fclose(file);
    modified = false; // Freshly loaded, so not modified yet
}

/**
 * @brief Saves the current buffer content to the file.
 */
void save_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Could not open file for writing");
        return;
    }

    Line *current = head;
    while (current != NULL)
    {
        fprintf(file, "%s\n", current->text);
        current = current->next;
    }
    fclose(file);
    printf("File '%s' saved.\n", filename);
    modified = false;
}

/**
 * @brief Frees all memory used by the text buffer.
 */
void free_buffer(void)
{
    Line *current = head;
    while (current != NULL)
    {
        Line *next = current->next;
        free(current->text); // Free the text string first
        free(current);       // Then free the node itself
        current = next;
    }
    head = tail = NULL;
    line_count = 0;
}

/**
 * @brief Prints all lines with line numbers.
 */
void print_lines(void)
{
    Line *current = head;
    int i = 1;
    while (current != NULL)
    {
        printf("%4d: %s\n", i++, current->text);
        current = current->next;
    }
}

/**
 * @brief Inserts a line at a specific position.
 */
void insert_line(int line_number, const char *text)
{
    if (line_number < 1 || line_number > line_count + 1)
    {
        printf("Error: Invalid line number.\n");
        return;
    }
    if (line_number == line_count + 1)
    {
        append_line(text);
        return;
    }

    Line *new_line = create_line(text);
    Line *current = head;
    for (int i = 1; i < line_number; i++)
    {
        current = current->next;
    }

    // Re-wire the pointers
    new_line->next = current;
    new_line->prev = current->prev;

    if (current->prev == NULL)
    { // Inserting at the head
        head = new_line;
    }
    else
    {
        current->prev->next = new_line;
    }
    current->prev = new_line;

    line_count++;
    modified = true;
}

/**
 * @brief Deletes a line at a specific position.
 */
void delete_line(int line_number)
{
    if (line_number < 1 || line_number > line_count)
    {
        printf("Error: Invalid line number.\n");
        return;
    }

    Line *to_delete = head;
    for (int i = 1; i < line_number; i++)
    {
        to_delete = to_delete->next;
    }

    // Bypass the node to be deleted
    if (to_delete->prev)
        to_delete->prev->next = to_delete->next;
    else
        head = to_delete->next; // Deleting the head

    if (to_delete->next)
        to_delete->next->prev = to_delete->prev;
    else
        tail = to_delete->prev; // Deleting the tail

    free(to_delete->text);
    free(to_delete);
    line_count--;
    modified = true;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Congratulations! You have built a complex, useful application from the ground up.
 * This project required careful management of memory, pointers, and program state,
 * exercising nearly every major C concept we have learned.
 *
 * Key Project Achievements:
 * - A DOUBLY-LINKED LIST implementation for efficient line editing.
 * - Robust FILE I/O for data persistence.
 * - A clean COMMAND-LINE INTERFACE for user interaction.
 * - Careful DYNAMIC MEMORY MANAGEMENT to prevent leaks.
 * - A modular structure that separates concerns into different functions.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 25_simple_text_editor 25_simple_text_editor.c`
 *
 * 4. Run the executable with a file name:
 *    - On Linux/macOS:   `./25_simple_text_editor my_document.txt`
 *    - On Windows:       `25_simple_text_editor.exe my_document.txt`
 *
 * Once inside, try the commands:
 * > a This is the first line.
 * > a This is the second.
 * > p
 * > i 2 This line goes in the middle.
 * > p
 * > d 1
 * > p
 * > s
 * > q
 */