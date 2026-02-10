/**
 * @file 12_structs.c
 * @brief Part 2, Lesson 12: Structs
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for structs.
 * It explains how to define and use custom data types to group related
 * variables, and how to access their members using both the dot (.) and
 * arrow (->) operators.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, our variables have held a single piece of data (like an `int` or a
 * `double`), and our arrays have held multiple pieces of the SAME type of data.
 *
 * But what if we want to represent a more complex, real-world object?
 * Think of a student. A student has a name (a string), an ID number (an integer),
 * and a GPA (a double). These are all different data types, but they all
 * logically belong together.
 *
 * This is where a STRUCT (short for structure) comes in. A STRUCT is a
 * user-defined data type that groups together related variables of potentially
 * different types into a single unit.
 */

#include <stdio.h>
#include <string.h> // We need this for strcpy() to handle strings

// --- Part 1: Defining a Struct ---

// We define the blueprint for our new data type using the `struct` keyword.
// This definition doesn't create any variables yet; it just tells the compiler
// what a "Student" looks like. It's like creating a blueprint for a house.
// By convention, struct names often start with a capital letter.
struct Student
{
    // These are the MEMBERS of the struct.
    char name[50];
    int student_id;
    double gpa;
};

// --- Part 2: Pointers to Structs ---

// Just like with any other data type (`int *`, `char *`), we can have pointers
// to structs. A pointer to a struct stores the memory address of a struct
// variable.
//
// When we want to access a struct's member through a pointer, we can't use the
// dot (.) operator directly. We use the ARROW OPERATOR (->).

int main(void)
{
    // --- DEMONSTRATION 1: Creating and Using a Struct Variable ---
    printf("--- Part 1: Creating and Accessing a Struct ---\n");

    // Now we can declare a variable of our new `struct Student` type.
    // This is like building an actual house from the blueprint.
    struct Student student1;

    // To access the members of a struct variable, we use the DOT OPERATOR (.).
    // Let's assign some values to student1's members.
    // For strings, we must use `strcpy` from <string.h> as we can't assign to an array directly.
    strcpy(student1.name, "Jane Doe");
    student1.student_id = 12345;
    student1.gpa = 3.8;

    // Now let's print the data to see our struct in action.
    printf("Student Name: %s\n", student1.name);
    printf("Student ID:   %d\n", student1.student_id);
    printf("Student GPA:  %.2f\n\n", student1.gpa); // .2f prints float/double to 2 decimal places

    // --- DEMONSTRATION 2: Using a Pointer to a Struct ---
    printf("--- Part 2: Pointers to Structs and the Arrow Operator ---\n");

    // Declare a pointer that can point to a `struct Student` variable.
    struct Student *p_student = NULL;

    // Assign the address of `student1` to our pointer.
    p_student = &student1;

    // Now we can access the members of `student1` through the pointer `p_student`.
    // We use the ARROW OPERATOR (->) for this. It's clean and intuitive.
    printf("Accessing data via pointer:\n");
    printf("Name: %s\n", p_student->name);
    printf("ID:   %d\n", p_student->student_id);
    printf("GPA:  %.2f\n\n", p_student->gpa);

    // We can also modify the original struct's data using the pointer.
    printf("Modifying GPA through the pointer...\n");
    p_student->gpa = 4.0;

    // Let's check the original student1 variable.
    printf("Original student1's new GPA: %.2f\n\n", student1.gpa); // It has been changed!

    // --- The Long Way: Dereferencing and Dot Operator ---
    // The arrow operator `->` is just "syntactic sugar" (a convenient shortcut).
    // The expression `p_student->gpa` is exactly equivalent to `(*p_student).gpa`.
    // This means "first, DEREFERENCE the pointer to get the actual struct, then
    // use the DOT OPERATOR on that struct."
    // The parentheses are required because the dot operator has higher precedence
    // than the dereference operator (*).
    printf("--- Bonus: The Long Way to Access Members via Pointer ---\n");
    printf("Accessing ID with (*p_student).student_id: %d\n", (*p_student).student_id);
    printf("See? It's the same, but `->` is much easier to read and type!\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  A STRUCT lets you create your own custom data types by grouping multiple
 *     variables (called MEMBERS) into one logical unit.
 * 2.  You first define the struct's blueprint, then you can declare variables
 *     of that new type.
 * 3.  Use the DOT OPERATOR (`.`) to access members of a struct variable directly.
 *     Example: `my_struct.member`
 * 4.  Use the ARROW OPERATOR (`->`) to access members of a struct through a pointer.
 *     Example: `my_pointer_to_struct->member`
 * 5.  The arrow operator `p->m` is a convenient shortcut for `(*p).m`.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 12_structs 12_structs.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./12_structs`
 *    - On Windows:       `12_structs.exe`
 */