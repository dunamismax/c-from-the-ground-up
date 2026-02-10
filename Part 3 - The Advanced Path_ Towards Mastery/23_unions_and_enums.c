/**
 * @file 23_unions_and_enums.c
 * @brief Part 3, Lesson 23: Unions and Enums
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for enums and unions.
 * It explains how these specialized data types are used for creating readable
 * constants and for memory-efficient data storage.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Today we're looking at two special kinds of data types: `enum` and `union`.
 * While they look a bit like `structs`, they solve very different problems.
 *
 * - ENUM (Enumeration):
 *   Its purpose is to make your code more READABLE and SAFER. An `enum` creates a
 *   set of named integer constants. Instead of using "magic numbers" like 0, 1, 2
 *   to represent states or types, you can use meaningful names like `SUCCESS`,
 *   `PENDING`, `FAILURE`.
 *
 * - UNION:
 *   Its purpose is MEMORY EFFICIENCY. A `union` allows you to store different
 *   data types in the SAME memory location, but only one at a time. While a
 *   `struct` allocates space for ALL its members, a `union` only allocates
 *   enough space for its LARGEST member.
 */

#include <stdio.h>
#include <stdint.h> // For fixed-width integers like uint8_t

// --- Part 1: Enums for Readable Code ---
// Let's define an enumeration for different status types in a system.
typedef enum
{
    STATE_PENDING,    // Automatically assigned the value 0
    STATE_PROCESSING, // Automatically assigned the value 1
    STATE_SUCCESS,    // Automatically assigned the value 2
    STATE_FAILURE     // Automatically assigned the value 3
} SystemState;

// --- Part 2: Unions for Memory Efficiency ---
// This union can hold ONE of three types, but not at the same time.
typedef union
{
    int i;
    double d;
    char c;
} DataValue;

// --- Part 3: The Tagged Union - The Best of Both Worlds ---
// A problem with unions is that you don't know WHICH member is currently valid.
// The solution is a TAGGED UNION. We wrap the union in a struct that also
// contains an enum "tag" to tell us what kind of data is currently stored.

typedef enum
{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_CHAR
} DataType;

typedef struct
{
    DataType type;   // The tag that tells us what's in the union
    DataValue value; // The union holding the actual data
} TaggedValue;

// A helper function to print our tagged union safely.
void print_tagged_value(TaggedValue tv)
{
    switch (tv.type)
    {
    case TYPE_INT:
        printf("TaggedValue is an INT: %d\n", tv.value.i);
        break;
    case TYPE_DOUBLE:
        printf("TaggedValue is a DOUBLE: %f\n", tv.value.d);
        break;
    case TYPE_CHAR:
        printf("TaggedValue is a CHAR: '%c'\n", tv.value.c);
        break;
    default:
        printf("TaggedValue has an unknown type.\n");
    }
}

int main(void)
{
    printf("--- Part 1: Using Enums ---\n");
    SystemState current_state = STATE_PROCESSING;

    // The code is much more readable than `if (current_state == 1)`
    if (current_state == STATE_PROCESSING)
    {
        printf("Current system state is: PROCESSING\n\n");
    }

    printf("--- Part 2: Using Unions ---\n");
    DataValue data;

    printf("Size of the DataValue union: %zu bytes\n", sizeof(DataValue));
    printf("Size of a double on this system: %zu bytes\n", sizeof(double));
    printf("The union's size matches its largest member!\n\n");

    // Store an int in the union
    data.i = 123;
    printf("Stored an int: data.i = %d\n", data.i);

    // Now store a double. THIS OVERWRITES THE INT'S MEMORY.
    data.d = 987.654;
    printf("Stored a double: data.d = %f\n", data.d);

    // The integer value is now corrupted because the same memory was reused.
    printf("The int value is now garbage: data.i = %d\n\n", data.i);

    printf("--- Part 3: Using Tagged Unions ---\n");

    // Create a tagged value to hold an integer
    TaggedValue int_val;
    int_val.type = TYPE_INT;
    int_val.value.i = 42;
    print_tagged_value(int_val);

    // Create another tagged value to hold a double
    TaggedValue double_val;
    double_val.type = TYPE_DOUBLE;
    double_val.value.d = 3.14159;
    print_tagged_value(double_val);

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  ENUMS create named integer constants to make your code more readable and
 *     less error-prone than using "magic numbers." They are perfect for `switch`
 *     statements or representing a fixed set of states.
 *
 * 2.  UNIONS allow multiple members to share the same memory location. This saves
 *     memory when you know you only need to store one of several possible data
 *     types at any given time.
 *
 * 3.  The most powerful pattern is the TAGGED UNION, where a `struct` contains
 *     both a `union` and an `enum` tag. The tag tells you which field of the
 *     union is currently active, allowing you to use it safely.
 *
 * Understanding when to use these specialized types is a mark of a proficient
 * C programmer.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 23_unions_and_enums 23_unions_and_enums.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./23_unions_and_enums`
 *    - On Windows:       `23_unions_and_enums.exe`
 */