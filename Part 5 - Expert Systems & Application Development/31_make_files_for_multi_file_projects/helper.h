/**
 * @file helper.h
 * @brief Part 5, Lesson 31: Header File for the Helper Module
 * @author dunamismax
 * @date 06-15-2025
 *
 * This is a HEADER FILE. Its purpose is to declare the functions and types
 * that another part of the program (in this case, `helper.c`) provides.
 * It acts as a public "interface" or "contract".
 */

/*
 * =====================================================================================
 * |                            - LESSON: HEADER FILES -                               |
 * =====================================================================================
 *
 * WHAT IS A HEADER FILE?
 * In a multi-file project, we need a way for one `.c` file to know about the
 * functions available in another `.c` file. A HEADER FILE (with a `.h` extension)
 * serves this purpose. It contains the DECLARATIONS of functions, but not their
 * actual code (the DEFINITIONS).
 *
 * Any `.c` file that wants to use these functions can simply `#include` this
 * header file.
 *
 * --- The Include Guard ---
 *
 * What happens if a header file gets included more than once in the same
 * compilation process? It can lead to errors ("redeclaration of '...''").
 * To prevent this, we use an INCLUDE GUARD. It's a standard preprocessor trick.
 *
 * `#ifndef HELPER_H`   // "If HELPER_H is NOT defined..."
 * `#define HELPER_H`   // "...then define it..."
 *
 *  // ... all the content of the header file goes here ...
 *
 * `#endif`             // "...and this is the end of the 'if' block."
 *
 * The first time the compiler sees this file, `HELPER_H` is not defined, so it
 * processes everything inside. It also defines `HELPER_H`. The second time it
 * sees this file, `HELPER_H` *is* defined, so the preprocessor skips everything
 * between `#ifndef` and `#endif`.
 *
 * The name `HELPER_H` should be unique to the file. A common convention is to
 * use the filename in all caps, replacing the `.` with an `_`.
 */

#ifndef HELPER_H
#define HELPER_H

// --- Function Prototypes ---

// This is a FUNCTION PROTOTYPE or FUNCTION DECLARATION.
// It tells the compiler everything it needs to know to *call* the function:
// its return type, its name, and the types of its parameters.
// The actual code for this function is in `helper.c`.
void say_hello_from_helper(void);

#endif // HELPER_H