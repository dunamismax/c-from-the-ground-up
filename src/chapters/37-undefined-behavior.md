# Undefined Behavior

> **What you'll learn:** what undefined behavior is, why optimizing compilers make it dangerous, the common kinds (signed overflow, out-of-bounds access, uninitialized reads, bad shifts, strict aliasing, unsequenced changes) and the safe way to write each.
>
> **Before you start:** Lessons 8, 13, 21 and 36.

The C standard describes what every valid program does. For some operations it
says the opposite: if a program does this, the standard places NO requirements
at all on what happens. That is UNDEFINED BEHAVIOR (UB). You have met it
already: reading past the end of an array (Lesson 8) and using memory after
`free` (Lesson 13).

"No requirements" is meant literally. The program may crash, print garbage, or
work perfectly today and fail after a compiler upgrade. Worse, OPTIMIZING
compilers ASSUME that undefined behavior never happens. A check such as
`if (x + 1 < x)` ("did `x + 1` overflow?") can be deleted entirely when `x` is
a signed `int`: the condition could only be true through an overflow, overflow
"cannot happen", so the compiler may treat it as always false.

So the rule is not "UB usually works out". The rule is: never let it happen.

## Signed Integer Overflow

Going past `INT_MAX` or `INT_MIN` (from `<limits.h>`) is SIGNED INTEGER
OVERFLOW, and it is undefined behavior. UNSIGNED arithmetic is different: it is
defined to WRAP AROUND, which Lesson 28's hash function relies on. The safe
way to add two `int`s is to check first, using only operations that cannot
overflow themselves:

```c
if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b))
{
    return false;
}
*result = a + b;
return true;
```

## Out-of-Bounds Access and Uninitialized Variables

Reading or writing outside an array is undefined behavior, even one element
past the end. Check an index before using it whenever it comes from outside
your own loop:

```c
if (index < 0 || index >= count)
{
    return false;
}
```

Using the value of a local variable that was never set is undefined behavior
too. That is why every lesson since Lesson 3 initializes its variables. `-Wall`
warns about many of these reads, and MemorySanitizer (`-fsanitize=memory`,
Clang on Linux) catches them at run time.

## Shifts

Shifting by a negative amount, by the width of the type or more (`1u << 32`
when an `unsigned int` has 32 bits), or shifting a negative value left is
undefined. Shift unsigned values, and check the amount first:

```c
int width = (int)(sizeof(unsigned int) * CHAR_BIT);
if (bit < 0 || bit >= width)
{
    return false;
}
*result = 1u << bit;
```

## Strict Aliasing

C lets you look at an object only through its own type, or as raw bytes.
Reading a `float` through a `uint32_t *`, as in
`uint32_t bits = *(uint32_t *)&value;`, breaks this STRICT ALIASING rule, and
the compiler may reorder or drop the read. To copy an object's bits into
another type, use `memcpy`, which copies bytes and is always allowed:

```c
memcpy(&bits, &value, sizeof(bits));
```

## Unsequenced Modifications

Changing a variable twice in one expression, as in `i = i++ + 1;`, or changing
it and reading it elsewhere in the same expression, is undefined: nothing says
which part happens first. Write one change per statement. `-Wall` warns about
these.

## Watching UBSan Catch It

UNDEFINEDBEHAVIORSANITIZER (UBSan, `-fsanitize=undefined`) checks for many of
these while the program runs. Building the lesson with `-DSHOW_UB` adds a real
signed overflow:

```c
int counter = INT_MAX;
counter = counter + 1; // UNDEFINED BEHAVIOR: signed overflow.
```

```sh
cc -g -Wall -Wextra -std=c17 -DSHOW_UB -fsanitize=undefined -o 37_ub 37_undefined_behavior.c
./37_ub
```

UBSan names the file, line and column, and the values involved: "signed
integer overflow: 2147483647 + 1 cannot be represented in type 'int'". Without
the sanitizer the program says nothing, and its result cannot be trusted.

## Full Source

<!-- source: Part 6 - The Toolsmith Path_ Debugging, UB and Testing/37_undefined_behavior.c -->
```c
/**
 * @file 37_undefined_behavior.c
 * @brief Part 6, Lesson 37: Undefined Behavior
 * @author dunamismax
 * @date 11-20-2026
 *
 * This file is a tour of UNDEFINED BEHAVIOR: what the C standard means by it,
 * why it is more dangerous than an ordinary bug, the most common kinds
 * (signed overflow, out-of-bounds access, uninitialized reads, bad shifts,
 * strict aliasing, unsequenced changes), the safe way to write each one, and
 * how UndefinedBehaviorSanitizer catches them at run time.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * The C standard describes what every valid program does. For some operations
 * it says the opposite: if a program does this, the standard places NO
 * requirements at all on what happens. That is UNDEFINED BEHAVIOR (UB). You
 * have met it already: reading past the end of an array (Lesson 8) and using
 * memory after `free` (Lesson 13).
 *
 * "No requirements" is meant literally. The program may crash, print garbage,
 * or work perfectly today and fail after a compiler upgrade or on another
 * machine. Worse, OPTIMIZING compilers ASSUME that undefined behavior never
 * happens and draw conclusions from that. A check such as
 *
 *    if (x + 1 < x)   // "did x + 1 overflow?"
 *
 * can be deleted entirely when `x` is a signed `int`: `x + 1 < x` could only
 * be true if the addition overflowed, overflow "cannot happen", so the
 * compiler may treat the condition as always false.
 *
 * So the rule is not "UB usually works out". The rule is: never let it
 * happen. This lesson shows the common kinds and the safe way to write each
 * one. UNDEFINEDBEHAVIORSANITIZER (UBSan, `-fsanitize=undefined`) catches many
 * of them while the program runs; Part 7 of this file shows it in action.
 */

#include <limits.h>  // INT_MAX, INT_MIN and CHAR_BIT
#include <stdbool.h> // bool, true and false
#include <stdint.h>  // uint32_t, an unsigned type of exactly 32 bits
#include <stdio.h>
#include <string.h> // memcpy

// --- Function Prototypes ---
bool checked_add(int a, int b, int *result);
bool element_at(const int values[], int count, int index, int *result);
bool bit_mask(int bit, unsigned int *result);

// --- Part 1: Signed Integer Overflow ---

/*
 * An `int` has a largest value, INT_MAX, and a smallest, INT_MIN (from
 * <limits.h>; 2147483647 and -2147483648 on every common machine). Going past
 * either one is SIGNED INTEGER OVERFLOW, and it is undefined behavior.
 *
 * UNSIGNED types are different: their arithmetic is defined to WRAP AROUND,
 * so `UINT_MAX + 1` is 0. Lesson 28's hash function relies on that.
 *
 * The safe way to add two `int`s is to check BEFORE adding whether the result
 * would fit, using only operations that cannot overflow themselves.
 */

/**
 * @brief Adds `a` and `b` into `*result` if the sum fits in an `int`.
 * @return true on success, false (leaving `*result` alone) if it would overflow.
 */
bool checked_add(int a, int b, int *result)
{
    // With a positive `b`, the sum is too big exactly when `a > INT_MAX - b`.
    // With a negative `b`, it is too small exactly when `a < INT_MIN - b`.
    // Neither subtraction can overflow: `b` has the sign that moves the limit
    // back toward zero.
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b))
    {
        return false;
    }
    *result = a + b;
    return true;
}

// --- Part 2: Out-of-Bounds Access ---

/*
 * Reading or writing outside an array is undefined behavior, even one element
 * past the end. Lesson 36 showed AddressSanitizer catching it. The safe
 * habit is to check an index before using it whenever it comes from outside
 * your own loop: from the user, a file or another function.
 */

/**
 * @brief Copies `values[index]` into `*result` if `index` is valid.
 */
bool element_at(const int values[], int count, int index, int *result)
{
    if (index < 0 || index >= count)
    {
        return false;
    }
    *result = values[index];
    return true;
}

// --- Part 3: Uninitialized Variables ---

/*
 * A local variable that has not been given a value holds whatever bytes were
 * in that memory before. Using its value is undefined behavior:
 *
 *    int count;
 *    printf("%d\n", count);   // WRONG: count was never set
 *
 * This is why every lesson since Lesson 3 initializes its variables (`int
 * user_age = 0;`). `-Wall` warns about many of these reads, and
 * MemorySanitizer (`-fsanitize=memory`, Clang on Linux) catches them at run
 * time.
 */

// --- Part 4: Shifts ---

/*
 * Shifting (Lesson 21) has three undefined cases: shifting by a negative
 * amount, shifting by the width of the type or more (`1u << 32` when an
 * `unsigned int` has 32 bits), and shifting a negative signed value left.
 * Shift unsigned values, and check the amount first.
 */

/**
 * @brief Stores a mask with only bit `bit` set in `*result`, if `bit` is valid.
 */
bool bit_mask(int bit, unsigned int *result)
{
    // The number of bits in an `unsigned int`: its size in bytes times
    // CHAR_BIT, the bits in a byte (8). The product is small, so it fits in
    // an `int`; the cast says so.
    int width = (int)(sizeof(unsigned int) * CHAR_BIT);
    if (bit < 0 || bit >= width)
    {
        return false;
    }
    *result = 1u << bit;
    return true;
}

// --- Part 5: Strict Aliasing ---

/*
 * C lets you look at an object only through its own type (or a compatible
 * one), or as raw bytes through `unsigned char`. Reading a `float` through a
 * `uint32_t *` breaks this STRICT ALIASING rule:
 *
 *    float value = 1.0f;
 *    uint32_t bits = *(uint32_t *)&value;   // WRONG: undefined behavior
 *
 * The compiler may assume a `uint32_t *` never points at a `float` and
 * reorder or drop the read. The safe way to copy an object's bits into
 * another type is `memcpy`, which copies bytes and is always allowed. Main
 * does exactly that below. (Compilers turn such a small `memcpy` into a
 * single move instruction, so it costs nothing.)
 */

// --- Part 6: Unsequenced Modifications ---

/*
 * Changing a variable twice in one expression, or changing it and reading it
 * elsewhere in the same expression, is undefined behavior:
 *
 *    i = i++ + 1;          // WRONG
 *    printf("%d %d\n", i, i++);   // WRONG
 *
 * There is no rule saying which part happens first. Write one change per
 * statement. `-Wall` warns about these (`-Wunsequenced` in Clang,
 * `-Wsequence-point` in GCC).
 */

int main(void)
{
    printf("--- Part 1: Signed Integer Overflow ---\n");
    int sum = 0;
    if (checked_add(2000000000, 100, &sum))
    {
        printf("2000000000 + 100 = %d\n", sum);
    }
    if (!checked_add(2000000000, 2000000000, &sum))
    {
        printf("2000000000 + 2000000000 would overflow an int, so it was refused.\n");
    }

    printf("\n--- Part 2: Out-of-Bounds Access ---\n");
    int values[] = {10, 20, 30, 40};
    int count = 4;
    int element = 0;
    if (element_at(values, count, 2, &element))
    {
        printf("values[2] = %d\n", element);
    }
    if (!element_at(values, count, 5, &element))
    {
        printf("Index 5 is outside an array of %d elements, so it was refused.\n", count);
    }

    printf("\n--- Part 4: Shifts ---\n");
    unsigned int mask = 0;
    if (bit_mask(3, &mask))
    {
        printf("bit_mask(3) = %u\n", mask);
    }
    if (!bit_mask(40, &mask))
    {
        printf("bit_mask(40) was refused: an unsigned int has only %d bits here.\n",
               (int)(sizeof(unsigned int) * CHAR_BIT));
    }

    printf("\n--- Part 5: Strict Aliasing ---\n");
    float value = 1.0f;
    uint32_t bits = 0;
    // Copy only if the two types really are the same size (they are on every
    // common machine), so memcpy can never write past the end of `bits`.
    if (sizeof(value) == sizeof(bits))
    {
        memcpy(&bits, &value, sizeof(bits));
        // `unsigned long` holds at least 32 bits, so it can print any uint32_t.
        printf("The bits of 1.0f are 0x%08lX.\n", (unsigned long)bits);
    }

    // --- Part 7: Watching UBSan Catch Undefined Behavior ---
    //
    // This block is compiled only when you build with `-DSHOW_UB` (conditional
    // compilation, Lesson 22). It overflows an `int` on purpose:
    //
    //    cc -g -Wall -Wextra -std=c17 -DSHOW_UB -fsanitize=undefined -o 37_ub 37_undefined_behavior.c
    //    ./37_ub
    //
    // UBSan prints a line like
    //
    //    37_undefined_behavior.c:<line>:<column>: runtime error: signed integer
    //    overflow: 2147483647 + 1 cannot be represented in type 'int'
    //
    // naming the file, line and column of the overflow. Without
    // `-fsanitize=undefined` the program says nothing, and whatever it prints
    // for the result cannot be trusted.
#ifdef SHOW_UB
    int counter = INT_MAX;
    counter = counter + 1; // UNDEFINED BEHAVIOR: signed overflow.
    printf("\nINT_MAX + 1 gave %d\n", counter);
#endif

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  UNDEFINED BEHAVIOR means the C standard places no requirements on what
 *     happens. "It worked when I tried it" proves nothing.
 * 2.  Optimizing compilers assume UB never happens, so they can remove checks
 *     that only make sense if it did.
 * 3.  Signed overflow is UB; unsigned arithmetic wraps around. Check before
 *     adding, as `checked_add` does.
 * 4.  Check indexes before using them, initialize every variable, and keep
 *     shift amounts between 0 and the type's width minus one.
 * 5.  Use `memcpy` to copy an object's bits into another type, never a
 *     pointer cast (STRICT ALIASING).
 * 6.  Change a variable at most once per expression.
 * 7.  Build with `-fsanitize=address,undefined` while developing, and turn
 *     on warnings: the compiler and the sanitizers catch most of these.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal. On Windows, use the Ubuntu terminal of WSL.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c17 -o 37_undefined_behavior 37_undefined_behavior.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./37_undefined_behavior`
 *    - On Windows (WSL): `./37_undefined_behavior`
 * 5. Then build it with `-DSHOW_UB -fsanitize=undefined` (see Part 7) and watch
 *    UBSan report the overflow.
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c17 -o 37_undefined_behavior 37_undefined_behavior.c
./37_undefined_behavior
```

## Try It Yourself

Practise what this lesson taught. Change a copy of the lesson file, or
start a new one.

1. Write `bool checked_multiply(int a, int b, int *result)` that refuses products that do not fit in an `int`. Test it with values near `INT_MAX` and with negative numbers.
2. Build the lesson with `-DSHOW_UB` once with `-O0` and once with `-O2`, without the sanitizer. Does the printed result change? Why can it?
3. Add a bad shift such as `1 << 40` behind its own `#ifdef`, build with `-fsanitize=undefined`, and read UBSan's report.
