# Testing in C

> **What you'll learn:** unit tests without a framework: `assert`, a CHECK macro that counts failures, exit codes for scripts and CI, edge cases, and the `#define main` trick for testing another program's functions.
>
> **Before you start:** Lessons 22, 36 and 37.

Every lesson in this course is checked automatically: a script compiles it,
feeds it input and compares what it prints with what it should print. That is
TESTING, and it is how you know code still works after you change it.

A UNIT TEST checks one small piece of code, usually one function, on chosen
inputs whose correct answers you already know: "`clamp(15, 0, 10)` must be
10". Many languages ship a testing library; in C you can go a long way with a
few lines of your own. The lesson tests three small functions: `clamp`,
`count_words` and `is_palindrome`.

## `assert`: Stating What Must Be True

`assert(condition)` from `<assert.h>` checks a condition while the program
runs. If it is false, the program prints the condition, the file and the line,
and stops at once. Use it for things that can only be false if the PROGRAM has
a bug, never for bad user input:

```c
assert(low <= high);
```

Compiling with `-DNDEBUG` removes every `assert`, so never put code that must
run inside one.

## A Tiny Test Harness

A TEST HARNESS is the code that runs tests and reports the results. The
lesson's is two counters and one macro:

```c
#define CHECK(condition)                                            \
    do                                                              \
    {                                                               \
        checks_run++;                                               \
        if (!(condition))                                           \
        {                                                           \
            checks_failed++;                                        \
            printf("FAILED (line %d): %s\n", __LINE__, #condition); \
        }                                                           \
    } while (0)
```

- `#condition` (the STRINGIZING operator) turns the argument into a string, so
  a failure shows the check exactly as written.
- `__LINE__` is replaced by the line number where the macro is used.
- `do { ... } while (0)` makes the macro's statements act like one statement,
  so it works after an `if` like any other statement.

Unlike `assert`, a failed CHECK does not stop the program, so one run reports
every failure.

## Edge Cases

Good tests check the ordinary case and the EDGE CASES, the inputs at the
boundaries where bugs hide:

```c
CHECK(count_words("hello world") == 2);
CHECK(count_words("") == 0);
CHECK(count_words("   ") == 0);
CHECK(count_words("  leading and trailing  ") == 3);
```

## Exit Status

`main` returns 0 when every check passed and 1 otherwise. Shells, Makefiles
and CI services read that EXIT STATUS, not the text. Try
`./38_testing; echo $?`.

## Testing Another Program's Functions

To test functions in a file that has its own `main`, such as Lesson 20's
linked list, rename that `main` before including the file:

```c
int lesson_20_main(void); // A prototype for the renamed main.
#define main lesson_20_main
#include "../Part 3 - The Advanced Path_ Towards Mastery/20_linked_lists.c"
#undef main
```

The included `main` becomes `lesson_20_main`, which nobody calls, and the test
file supplies the real `main`. The repository does exactly this in
`tests/test_linked_list.c` and `tests/test_hash_table.c` (Lesson 28), and the
course's own checks compile and run them on every change.

## Full Source

<!-- source: Part 6 - The Toolsmith Path_ Debugging, UB and Testing/38_testing.c -->
```c
/**
 * @file 38_testing.c
 * @brief Part 6, Lesson 38: Testing in C
 * @author dunamismax
 * @date 11-20-2026
 *
 * This file teaches how to test C code without any framework: `assert` for
 * conditions that must never be false, a tiny CHECK macro that counts
 * failures, exit codes that tell scripts whether the tests passed, and the
 * `#define main` trick for testing the functions of another program.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Every lesson in this course is checked automatically: a script compiles it,
 * feeds it input and compares what it prints with what it should print. That
 * is TESTING, and it is how you know code still works after you change it.
 *
 * A UNIT TEST checks one small piece of code, usually one function, on chosen
 * inputs whose correct answers you already know: "clamp(15, 0, 10) must be
 * 10". When you later change `clamp`, the tests tell you at once if you broke
 * it. Many languages ship a testing library; in C you can go a long way with
 * a few lines of your own.
 *
 * This lesson has three small functions to test, a CHECK macro, a test
 * function for each, and a `main` that runs them all.
 */

#include <assert.h>  // assert
#include <ctype.h>   // isspace
#include <stdbool.h> // bool, true and false
#include <stdio.h>
#include <string.h> // strlen

// --- Function Prototypes ---
int clamp(int value, int low, int high);
int count_words(const char *text);
bool is_palindrome(const char *text);
void test_clamp(void);
void test_count_words(void);
void test_is_palindrome(void);

// --- Part 1: The Code Under Test ---

/*
 * ASSERT: STATING WHAT MUST BE TRUE
 *
 * `assert(condition)` from <assert.h> checks a condition while the program
 * runs. If it is false, the program prints the condition, the file and the
 * line, and stops at once (with `abort`). Use it for things that can only be
 * false if the PROGRAM has a bug, such as a function being called with
 * arguments that make no sense. It is not for bad user input: input errors
 * deserve a friendly message, not a crash.
 *
 * Compiling with `-DNDEBUG` ("no debug") removes every `assert`, so never
 * put code that must run inside one.
 */

/**
 * @brief Limits `value` to the range `low` to `high`.
 */
int clamp(int value, int low, int high)
{
    // Asking for a range whose low end is above its high end is a bug in
    // the caller, so it is an assert, not an error message.
    assert(low <= high);

    if (value < low)
    {
        return low;
    }
    if (value > high)
    {
        return high;
    }
    return value;
}

/**
 * @brief Counts the words in `text`: runs of characters that are not spaces,
 * tabs or newlines.
 */
int count_words(const char *text)
{
    int words = 0;
    bool in_word = false;

    for (size_t i = 0; text[i] != '\0'; i++)
    {
        // `isspace` needs a value that fits in `unsigned char` (or EOF), so the
        // character is converted first; a negative `char` would be undefined
        // behavior (Lesson 37).
        if (isspace((unsigned char)text[i]))
        {
            in_word = false;
        }
        else if (!in_word)
        {
            in_word = true; // The first character of a new word.
            words++;
        }
    }
    return words;
}

/**
 * @brief Returns true if `text` reads the same forwards and backwards.
 */
bool is_palindrome(const char *text)
{
    size_t length = strlen(text);
    if (length == 0)
    {
        return true;
    }

    // Compare the first character with the last, the second with the second
    // to last, and so on, moving inward until the two positions meet.
    size_t left = 0;
    size_t right = length - 1;
    while (left < right)
    {
        if (text[left] != text[right])
        {
            return false;
        }
        left++;
        right--;
    }
    return true;
}

// --- Part 2: A Tiny Test Harness ---

/*
 * A TEST HARNESS is the code that runs tests and reports the results. Ours is
 * two counters and one macro.
 *
 * CHECK(condition) counts one check, and if `condition` is false it counts a
 * failure and prints which check failed and on which line. Three preprocessor
 * features (Lesson 22) make this work:
 *
 * - `#condition` (the STRINGIZING operator) turns the macro's argument into a
 *   string, so the failure message shows the check exactly as you wrote it.
 * - `__LINE__` is replaced by the line number where the macro is used.
 * - `do { ... } while (0)` makes the macro's several statements act like ONE
 *   statement, so `if (x) CHECK(y); else ...` works as it looks. The loop runs
 *   exactly once.
 *
 * The backslash at the end of each line continues the macro onto the next.
 * Unlike `assert`, a failed CHECK does not stop the program, so one run
 * reports every failure.
 */

static int checks_run = 0;
static int checks_failed = 0;

#define CHECK(condition)                                            \
    do                                                              \
    {                                                               \
        checks_run++;                                               \
        if (!(condition))                                           \
        {                                                           \
            checks_failed++;                                        \
            printf("FAILED (line %d): %s\n", __LINE__, #condition); \
        }                                                           \
    } while (0)

// --- Part 3: The Tests ---

/*
 * Good tests check the ordinary case AND the EDGE CASES, the inputs at the
 * boundaries where bugs hide: values exactly at a limit, empty strings, one
 * character, extra spaces.
 */

void test_clamp(void)
{
    CHECK(clamp(5, 0, 10) == 5);   // Inside the range: unchanged.
    CHECK(clamp(-3, 0, 10) == 0);  // Below: raised to the low end.
    CHECK(clamp(15, 0, 10) == 10); // Above: lowered to the high end.
    CHECK(clamp(0, 0, 10) == 0);   // Exactly at each end.
    CHECK(clamp(10, 0, 10) == 10);
    CHECK(clamp(7, 7, 7) == 7); // A range of one value.
}

void test_count_words(void)
{
    CHECK(count_words("hello world") == 2);
    CHECK(count_words("") == 0);
    CHECK(count_words("   ") == 0);
    CHECK(count_words("one") == 1);
    CHECK(count_words("  leading and trailing  ") == 3);
    CHECK(count_words("tabs\tand\nnewlines") == 3);
}

void test_is_palindrome(void)
{
    CHECK(is_palindrome("racecar"));
    CHECK(is_palindrome("abba"));
    CHECK(is_palindrome("a"));
    CHECK(is_palindrome(""));
    CHECK(!is_palindrome("ab"));
    CHECK(!is_palindrome("Racecar")); // Case matters: 'R' is not 'r'.
}

// --- Part 4: Running the Tests ---

/*
 * `main` runs every test function and reports a summary. Its EXIT STATUS is
 * the important part: 0 means every check passed, anything else means a
 * failure. Shells, Makefiles and CI services read that number, not the text.
 * Try `./38_testing; echo $?` to see it.
 */

int main(void)
{
    test_clamp();
    test_count_words();
    test_is_palindrome();

    if (checks_failed > 0)
    {
        printf("%d of %d checks FAILED.\n", checks_failed, checks_run);
        return 1;
    }
    printf("All %d checks passed.\n", checks_run);
    return 0;
}

/*
 * --- Part 5: Testing Another Program's Functions ---
 *
 * The functions above live in the same file as their tests. Often you want
 * to test functions in a file that already has its own `main`, such as
 * Lesson 20's linked list. Compiling the two files together fails: a program
 * can have only one `main`.
 *
 * The preprocessor has a trick for this. Rename the other file's `main`
 * before including it:
 *
 *    int lesson_20_main(void);   // a prototype for the renamed main
 *    #define main lesson_20_main
 *    #include "20_linked_lists.c"
 *    #undef main
 *
 *    int main(void)
 *    {
 *        Node *head = NULL;
 *        insert_at_beginning(&head, 1);
 *        ...
 *    }
 *
 * Every `main` inside the included file becomes `lesson_20_main`, which
 * nobody calls, and the test file supplies the real `main`. The repository
 * does exactly this in `tests/test_linked_list.c` (Lesson 20) and
 * `tests/test_hash_table.c` (Lesson 28); the course's own checks compile and
 * run them on every change.
 *
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  A UNIT TEST runs one function on inputs whose answers you know and
 *     checks the results. Test the EDGE CASES, not just the ordinary ones.
 * 2.  `assert` stops the program when something that must be true is false.
 *     Use it for programmer mistakes, never for user input, and remember that
 *     `-DNDEBUG` removes it.
 * 3.  A CHECK macro built from `#condition`, `__LINE__` and `do { } while (0)`
 *     records failures and keeps going, so one run reports them all.
 * 4.  Return 0 from `main` when every test passes and non-zero otherwise:
 *     scripts and CI read the exit status.
 * 5.  `#define main other_name` before an `#include` lets a test file use the
 *     functions of a program that has its own `main`.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal. On Windows, use the Ubuntu terminal of WSL.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c17 -o 38_testing 38_testing.c`
 * 4. Run the executable and print its exit status:
 *    - On Linux/macOS:   `./38_testing; echo $?`
 *    - On Windows (WSL): `./38_testing; echo $?`
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c17 -o 38_testing 38_testing.c
./38_testing
```

## Try It Yourself

Practise what this lesson taught. Change a copy of the lesson file, or
start a new one.

1. Break `clamp` on purpose (swap `low` and `high` in one `return`) and run the tests. Do the failure messages point you to the bug? Then fix it.
2. Write `int count_vowels(const char *text)` and tests for it, including the empty string and a string with no vowels.
3. Add a test of your own to `tests/test_linked_list.c`, for example one for an `insert_at_end` function you write for Lesson 20.
