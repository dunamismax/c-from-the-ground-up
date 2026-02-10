/**
 * @file 21_bit_manipulation.c
 * @brief Part 3, Lesson 21: Bit Manipulation
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for bit manipulation. It
 * introduces the bitwise operators and shows how to directly manipulate the
 * individual bits of data, a powerful technique for optimization and control.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * C is a language that operates very close to the hardware. All data in your
 * computer—integers, characters, etc.—is ultimately stored as a sequence of
 * BITS (binary digits), which are either 0 or 1.
 *
 * C provides a special set of BITWISE OPERATORS that allow you to manipulate
 * these individual bits directly. This is a low-level feature that is incredibly
 * powerful for tasks like:
 * - Controlling hardware devices where each bit in a register has a meaning.
 * - Writing highly efficient code for specific algorithms.
 * - Storing multiple boolean (true/false) flags in a single byte.
 *
 * --- The Bitwise Operators ---
 *
 * &  (Bitwise AND):     Sets a bit to 1 if BOTH corresponding bits are 1.
 * |  (Bitwise OR):      Sets a bit to 1 if EITHER corresponding bit is 1.
 * ^  (Bitwise XOR):     Sets a bit to 1 if the corresponding bits are DIFFERENT.
 * ~  (Bitwise NOT):     Inverts all the bits (0s become 1s and 1s become 0s).
 * << (Left Shift):      Shifts all bits to the left by a specified number of places.
 * >> (Right Shift):     Shifts all bits to the right by a specified number of places.
 */

#include <stdio.h>
#include <stdint.h> // For uint8_t, an unsigned 8-bit integer type.

// --- A Helper Function to Visualize Bits ---
// To understand bit manipulation, you need to SEE the bits. This function
// will print the binary representation of a byte (an 8-bit number).
// We use `uint8_t` for a clear, 8-bit unsigned integer.
void print_binary(uint8_t byte)
{
    printf("0b"); // "0b" prefix indicates a binary number
    // We loop from the most significant bit (7) down to the least (0).
    for (int i = 7; i >= 0; i--)
    {
        // `(byte >> i)` shifts the bit we're interested in to the 0th position.
        // `& 1` then isolates that bit. If it's 1, the result is 1; otherwise, 0.
        printf("%d", (byte >> i) & 1);
    }
}

int main(void)
{
    // Let's use two small numbers for our demonstration.
    uint8_t a = 5; // Binary: 00000101
    uint8_t b = 9; // Binary: 00001001

    printf("--- Part 1: Basic Bitwise Operations ---\n");
    printf("a = %2d = ", a);
    print_binary(a);
    printf("\n");
    printf("b = %2d = ", b);
    print_binary(b);
    printf("\n\n");

    // Bitwise AND
    uint8_t result_and = a & b; // 00000101 & 00001001 = 00000001 (1)
    printf("a & b = %2d = ", result_and);
    print_binary(result_and);
    printf("\n");

    // Bitwise OR
    uint8_t result_or = a | b; // 00000101 | 00001001 = 00001101 (13)
    printf("a | b = %2d = ", result_or);
    print_binary(result_or);
    printf("\n");

    // Bitwise XOR
    uint8_t result_xor = a ^ b; // 00000101 ^ 00001001 = 00001100 (12)
    printf("a ^ b = %2d = ", result_xor);
    print_binary(result_xor);
    printf("\n");

    // Bitwise NOT
    uint8_t result_not_a = ~a; // ~00000101 = 11111010 (250)
    printf("  ~a  = %3d = ", result_not_a);
    print_binary(result_not_a);
    printf("\n\n");

    // --- Part 2: Shift Operations ---
    printf("--- Part 2: Shift Operations ---\n");
    uint8_t c = 12; // Binary: 00001100

    printf("c = %2d = ", c);
    print_binary(c);
    printf("\n");

    // Left Shift (equivalent to multiplying by 2^n)
    uint8_t left_shifted = c << 2; // 12 * 2^2 = 12 * 4 = 48 (00110000)
    printf("c << 2 = %2d = ", left_shifted);
    print_binary(left_shifted);
    printf("\n");

    // Right Shift (equivalent to integer division by 2^n)
    uint8_t right_shifted = c >> 1; // 12 / 2^1 = 12 / 2 = 6 (00000110)
    printf("c >> 1 = %2d = ", right_shifted);
    print_binary(right_shifted);
    printf("\n\n");

    // --- Part 3: Practical Use - BITMASKS for Flags ---
    // A very common use case is to store multiple on/off states in a single byte.
    // Each bit position represents a different flag.

    printf("--- Part 3: Practical Bitmasking ---\n");
    const uint8_t FLAG_READ_ACCESS = 1 << 0;    // 00000001 (1)
    const uint8_t FLAG_WRITE_ACCESS = 1 << 1;   // 00000010 (2)
    const uint8_t FLAG_EXECUTE_ACCESS = 1 << 2; // 00000100 (4)
    const uint8_t FLAG_IS_ADMIN = 1 << 7;       // 10000000 (128)

    uint8_t user_permissions = 0; // Start with no permissions.
    printf("Initial permissions: ");
    print_binary(user_permissions);
    printf("\n");

    // 1. SETTING a bit (giving permission) using OR
    user_permissions = user_permissions | FLAG_READ_ACCESS;
    user_permissions = user_permissions | FLAG_WRITE_ACCESS;
    printf("After giving R/W:    ");
    print_binary(user_permissions);
    printf("\n");

    // 2. CHECKING a bit (do they have permission?) using AND
    if (user_permissions & FLAG_WRITE_ACCESS)
    {
        printf("User has WRITE access.\n");
    }
    if (!(user_permissions & FLAG_EXECUTE_ACCESS))
    {
        printf("User does NOT have EXECUTE access.\n");
    }

    // 3. CLEARING a bit (revoking permission) using AND with NOT
    user_permissions = user_permissions & ~FLAG_WRITE_ACCESS;
    printf("After revoking W:    ");
    print_binary(user_permissions);
    printf("\n");

    // 4. TOGGLING a bit (flipping its state) using XOR
    user_permissions = user_permissions ^ FLAG_READ_ACCESS; // Turn it off
    user_permissions = user_permissions ^ FLAG_READ_ACCESS; // Turn it back on
    printf("After toggling R twice:");
    print_binary(user_permissions);
    printf("\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  Bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`) allow you to directly
 *     manipulate the 0s and 1s that make up numeric data types.
 * 2.  A BITMASK is a value (often a constant) used to target specific bits in another
 *     variable for setting, clearing, or checking.
 * 3.  The common patterns for bitmasking are:
 *     - Set bit:      `flags = flags | MASK;`
 *     - Clear bit:    `flags = flags & ~MASK;`
 *     - Check bit:    `if (flags & MASK) { ... }`
 *     - Toggle bit:   `flags = flags ^ MASK;`
 * 4.  Shift operators provide a very fast way to perform multiplication and
 *     division by powers of two.
 *
 * This is a low-level but essential skill for any serious C programmer.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 21_bit_manipulation 21_bit_manipulation.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./21_bit_manipulation`
 *    - On Windows:       `21_bit_manipulation.exe`
 */