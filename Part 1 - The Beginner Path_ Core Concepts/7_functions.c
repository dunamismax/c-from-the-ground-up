/**
 * @file 7_functions.c
 * @brief Part 1, Lesson 7: Functions
 * @author dunamismax
 * @date 06-15-2025
 *
 * This lesson introduces functions, which are the fundamental building blocks
 * for organizing code into reusable, modular pieces.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * As programs grow larger, putting all the code inside the `main` function
 * becomes messy, hard to read, and difficult to maintain. If you have a task
 * that needs to be performed multiple times, you don't want to copy and paste
 * the same block of code over and over.
 *
 * A FUNCTION is a named, self-contained block of code that performs a specific
 * task. Think of `printf` and `scanf`—they are functions that someone else wrote
 * to perform the tasks of printing and scanning. Now, you will learn to write
 * your own.
 *
 * Using functions allows us to follow the "DRY" principle: Don't Repeat Yourself.
 *
 * There are three key parts to using a function:
 * 1. The DECLARATION (also called the PROTOTYPE): Informs the compiler about the
 *    function's existence, its name, what type of value it returns, and what
 *    parameters it accepts.
 * 2. The DEFINITION: The actual code that makes up the function's body.
 * 3. The CALL: The statement that executes the function.
 */

#include <stdio.h>

// --- Part 1: Function Declarations (Prototypes) ---

/*
 * A FUNCTION PROTOTYPE is a declaration that tells the compiler what a function
 * looks like *before* it's actually defined. This is crucial because the compiler
 * reads files from top to bottom. If `main` tries to call a function that the
 * compiler hasn't seen yet, it will generate an error. Prototypes solve this.
 *
 * A prototype consists of:
 * - The return type of the function.
 * - The name of the function.
 * - The data types of the parameters it accepts, in order.
 * - A semicolon at the end.
 */

// Prototype for a simple function that takes nothing and returns nothing.
void print_welcome_message(void);

// Prototype for a function that takes an integer as input and returns nothing.
// The name `character_level` inside the parentheses is optional in a prototype,
// but can be good for documentation.
void display_character_level(int character_level);

// Prototype for a function that takes two integers and returns an integer.
int calculate_damage(int strength, int weapon_power);

// --- Part 2: The `main` Function (The Caller) ---

/*
 * The `main` function will now act as a manager, calling our other functions
 * to perform their specific tasks.
 */
int main(void)
{
    // FUNCTION CALL: To execute a function, you write its name followed by
    // parentheses. This is a call to `print_welcome_message`.
    print_welcome_message();

    int player_level = 15;
    // This call passes the *value* of `player_level` (which is 15) as an
    // ARGUMENT to the `display_character_level` function.
    display_character_level(player_level);

    // Here, we call `calculate_damage` and store its result.
    // The values 20 and 8 are the ARGUMENTS we pass.
    int total_damage = calculate_damage(20, 8);

    printf("With 20 strength and a weapon power of 8...\n");
    printf("Total calculated damage is: %d\n", total_damage);

    return 0; // End the main program
}

// --- Part 3: Function Definitions (The Implementation) ---

/*
 * Below `main`, we provide the full DEFINITIONS for our functions. A definition
 * includes the function's header (which looks like the prototype, but without
 * the semicolon) and its body inside curly braces `{}`.
 */

/**
 * @brief Prints a standard welcome message to the console.
 *
 * This is an example of a function that takes no arguments and returns no value.
 * `void` as a return type means it doesn't send any value back.
 * `(void)` as the parameter list means it doesn't accept any input.
 */
void print_welcome_message(void)
{
    printf("--- Welcome to the Function Demo! ---\n\n");
}

/**
 * @brief Displays a character's level.
 * @param character_level The level to be displayed.
 *
 * This function accepts one piece of data: an integer. Inside this function,
 * that integer is known by the PARAMETER name `character_level`. A PARAMETER is a
 * special local variable that holds the value of the ARGUMENT passed during the call.
 */
void display_character_level(int character_level)
{
    printf("Your character is level %d.\n\n", character_level);
}

/**
 * @brief Calculates damage based on strength and weapon power.
 * @param strength The character's strength attribute.
 * @param weapon_power The power rating of the equipped weapon.
 * @return The total calculated damage as an integer.
 *
 * This function takes two integers as input and, importantly, it has a
 * RETURN VALUE. The `return` keyword does two things:
 * 1. It immediately ends the execution of this function.
 * 2. It sends the value that follows it back to wherever the function was called.
 */
int calculate_damage(int strength, int weapon_power)
{
    int calculated_value = strength + (weapon_power * 2);
    return calculated_value; // Send this value back to the caller (`main`)
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * In this lesson, you've unlocked one of the most powerful organizational tools in C.
 *
 * Key Takeaways:
 * - Functions let you break down large problems into smaller, manageable, and
 *   reusable pieces of code.
 * - A FUNCTION PROTOTYPE (declaration) tells the compiler a function exists before it's defined.
 * - A FUNCTION DEFINITION contains the actual code that runs.
 * - A FUNCTION CALL executes the function.
 * - PARAMETERS are the variables in a function's definition that accept input.
 * - ARGUMENTS are the actual values you pass to a function when you call it.
 * - The `return` keyword sends a value back from the function to the caller.
 * - A `void` return type means the function does not return any value.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 7_functions 7_functions.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./7_functions`
 *    - On Windows:       `7_functions.exe`
 */