# Debugging

> **What you'll learn:** building with debug information, stopping and stepping through a program in gdb or lldb, reading a backtrace, and reading an AddressSanitizer report.
>
> **Before you start:** The whole course through Lesson 35; Lessons 8, 13 and 22 matter most.

Welcome to Part 6, the Toolsmith Path. You can now write real C programs.
This part is about the tools that keep them correct: debuggers, sanitizers and
tests.

Until now, when a program misbehaved, the natural move was to add `printf`
calls and run it again. That works, but it is slow: every new question means
editing, recompiling and rerunning. A DEBUGGER answers those questions while
the program runs. It can stop the program at a line you choose (a
BREAKPOINT), run it one line at a time (STEPPING), show the value of any
variable, and show the chain of function calls that led to the current line
(the BACKTRACE).

The two common debuggers are GDB (usual on Linux and WSL) and LLDB (which
macOS ships with the Xcode command line tools). They work the same way; only
some command names differ. On Linux, install gdb with `sudo apt install gdb`.

## Build With Debug Information

A debugger needs a map from the machine code back to your source: file names,
line numbers and variable names. The `-g` flag adds this DEBUG INFORMATION.
`-O0` turns optimization off, so the program runs line by line exactly as you
wrote it:

```sh
cc -g -O0 -Wall -Wextra -std=c17 -o 36_debugging 36_debugging.c
```

The lesson's program analyzes a week of temperature readings. As written, it
is correct:

```c
int readings[DAYS] = {18, 21, 19, 24, 22, 17, 20};

int total = sum_readings(readings, DAYS);
int highest = highest_reading(readings, DAYS);
double average = average_reading(readings, DAYS);
```

## A Debugging Session

Start the program under the debugger (`gdb ./36_debugging` or
`lldb ./36_debugging`) and type commands at its prompt:

| What you want | GDB | LLDB |
| --- | --- | --- |
| Stop when a function starts | `break sum_readings` | `b sum_readings` |
| Start the program | `run` | `run` |
| Run the next line | `next` | `next` |
| Go into the function called on this line | `step` | `step` |
| Print a variable | `print total` | `p total` |
| Show every local variable | `info locals` | `frame variable` |
| Show the chain of calls | `backtrace` | `bt` |
| Run until this function returns | `finish` | `finish` |
| Stop whenever a variable changes | `watch total` | `watchpoint set variable total` |
| Keep running to the next breakpoint | `continue` | `continue` |
| Leave the debugger | `quit` | `quit` |

Try it: `break sum_readings`, `run`, then `next` and `print total` a few times
to watch the total grow. `backtrace` shows two FRAMES, `sum_readings` called by
`main`; `finish` shows the return value, 141. `continue` stops in
`sum_readings` again, because `average_reading` calls it too, and now the
backtrace has three frames.

A backtrace is most useful when a program crashes: run it in the debugger,
and when it stops, `backtrace` shows the line that crashed (frame #0) and how
the program got there.

## Catching the Hidden Bug

`sum_readings` has a bug you can switch on with `-DSHOW_BUG` (conditional
compilation, Lesson 22). It makes the loop stop at index `count` instead of
`count - 1`, the classic OFF-BY-ONE error, so the last pass reads one element
past the end of the array:

```c
#ifdef SHOW_BUG
    int last = count;
#else
    int last = count - 1; // The last valid index.
#endif
```

That is undefined behavior (Lesson 8). The program may print a wrong total or,
by luck, the right one, and it never says anything is wrong. A SANITIZER is a
checker the compiler builds into your program. ADDRESSSANITIZER (ASan,
`-fsanitize=address`) checks every memory access and stops the program at the
first bad one:

```sh
cc -g -O0 -Wall -Wextra -std=c17 -DSHOW_BUG -fsanitize=address -o 36_debugging_bug 36_debugging.c
./36_debugging_bug
```

Shortened, its report looks like this (addresses and line numbers differ):

```text
==12345==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffc...
READ of size 4 at 0x7ffc... thread T0
    #0 0x... in sum_readings 36_debugging.c:<line>
    #1 0x... in main 36_debugging.c:<line>
...
    [32, 60) 'readings' <== Memory access at offset 60 overflows this variable
```

Read it from the top: the kind of error (an overflow of an array on the
stack), what happened (a READ of 4 bytes, one `int`), the backtrace (frame #0,
`sum_readings`, is where it happened), and the variable it went past,
`readings`.

## Full Source

<!-- source: Part 6 - The Toolsmith Path_ Debugging, UB and Testing/36_debugging.c -->
```c
/**
 * @file 36_debugging.c
 * @brief Part 6, Lesson 36: Debugging
 * @author dunamismax
 * @date 11-20-2026
 *
 * This file teaches how to find bugs with tools instead of guesswork: building
 * with debug information, pausing and stepping through a program in a
 * DEBUGGER (gdb or lldb), reading a BACKTRACE, and reading the report a
 * SANITIZER prints when it catches a memory error.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Welcome to Part 6, the Toolsmith Path. You can now write real C programs.
 * This part is about the tools that keep them correct: debuggers, sanitizers
 * and tests.
 *
 * Until now, when a program misbehaved, the natural move was to add `printf`
 * calls and run it again. That works, but it is slow: every new question means
 * editing, recompiling and rerunning. A DEBUGGER answers those questions while
 * the program runs. It can:
 *
 * - stop the program at a line you choose (a BREAKPOINT),
 * - run it one line at a time (STEPPING),
 * - show the value of any variable at that moment,
 * - show the chain of function calls that led to the current line (the
 *   BACKTRACE, also called the call stack).
 *
 * The two common debuggers are GDB (the GNU debugger, usual on Linux and WSL)
 * and LLDB (the LLVM debugger, the one macOS ships with the Xcode command line
 * tools). They work the same way; only some command names differ. On Linux,
 * install gdb with `sudo apt install gdb` (or your system's package manager).
 *
 * STEP 1: BUILD WITH DEBUG INFORMATION
 *
 * A debugger needs a map from the machine code back to your source: file
 * names, line numbers and variable names. The `-g` flag adds this DEBUG
 * INFORMATION. The `-O0` flag turns optimization off, so the program runs
 * line by line exactly as you wrote it (an optimized program can skip lines or
 * keep a variable only in a register, which makes stepping confusing):
 *
 *    cc -g -O0 -Wall -Wextra -std=c17 -o 36_debugging 36_debugging.c
 *
 * The program below analyzes a week of temperature readings. As written, it
 * is correct. Later in the lesson you will switch on a hidden bug and catch it
 * with a sanitizer.
 */

#include <stdio.h>

#define DAYS 7

// --- Function Prototypes ---
int sum_readings(const int readings[], int count);
int highest_reading(const int readings[], int count);
double average_reading(const int readings[], int count);

// --- Part 1: The Program We Will Debug ---

/**
 * @brief Adds up `count` readings.
 */
int sum_readings(const int readings[], int count)
{
    int total = 0;

    // The loop bound is where the hidden bug lives. Compiling with
    // `-DSHOW_BUG` defines the macro SHOW_BUG, and conditional compilation
    // (Lesson 22) then picks the buggy line instead of the correct one.
#ifdef SHOW_BUG
    // THE BUG: the last valid index of an array of `count` elements is
    // `count - 1`, but this version stops at `count`, so the final pass reads
    // `readings[count]`, one element past the end. This is the classic
    // OFF-BY-ONE error, the same mistake as writing `i <= count` in the loop.
    int last = count;
#else
    int last = count - 1; // The last valid index.
#endif
    for (int i = 0; i <= last; i++)
    {
        total += readings[i];
    }
    return total;
}

/**
 * @brief Returns the largest of `count` readings. `count` must be at least 1.
 */
int highest_reading(const int readings[], int count)
{
    int highest = readings[0];
    for (int i = 1; i < count; i++)
    {
        if (readings[i] > highest)
        {
            highest = readings[i];
        }
    }
    return highest;
}

/**
 * @brief Returns the average of `count` readings. `count` must be at least 1.
 */
double average_reading(const int readings[], int count)
{
    return (double)sum_readings(readings, count) / count;
}

int main(void)
{
    int readings[DAYS] = {18, 21, 19, 24, 22, 17, 20};

    int total = sum_readings(readings, DAYS);
    int highest = highest_reading(readings, DAYS);
    double average = average_reading(readings, DAYS);

    printf("Readings for %d days\n", DAYS);
    printf("Total:   %d\n", total);
    printf("Highest: %d\n", highest);
    printf("Average: %.2f\n", average);

    return 0;
}

/*
 * --- Part 2: A Debugging Session ---
 *
 * Build the program with `-g -O0` as shown above, then start it under the
 * debugger. Each line below is one command you type at the debugger's prompt:
 * `(gdb)` or `(lldb)`.
 *
 *    gdb ./36_debugging              lldb ./36_debugging
 *
 *    WHAT YOU WANT                   GDB                    LLDB
 *    Stop when a function starts     break sum_readings     b sum_readings
 *    Start the program               run                    run
 *    Run the next line               next                   next
 *    Go INTO the function called     step                   step
 *      on this line
 *    Print a variable                print total            p total
 *    Print an array element          print readings[i]      p readings[i]
 *    Show every local variable       info locals            frame variable
 *    Show the chain of calls         backtrace              bt
 *    Run until this function         finish                 finish
 *      returns
 *    Stop whenever a variable        watch total            watchpoint set
 *      changes                                                variable total
 *    Keep running until the next     continue               continue
 *      breakpoint
 *    Leave the debugger              quit                   quit
 *
 * Try this session:
 *
 * 1. `break sum_readings`, then `run`. The program stops at the first line of
 *    `sum_readings`, BEFORE that line runs.
 * 2. `next` a few times and `print total` after each one. Watch `total` grow
 *    by one reading per pass of the loop.
 * 3. `backtrace`. You will see two FRAMES: `sum_readings` on top, called by
 *    `main` below it. Each frame lists its arguments, so you can see that
 *    `count` is 7.
 * 4. `finish` runs to the end of `sum_readings` and shows its return value,
 *    141.
 * 5. `continue`. The program stops in `sum_readings` again, because
 *    `average_reading` calls it too. `backtrace` now shows three frames:
 *    `sum_readings`, `average_reading`, `main`.
 * 6. `quit`.
 *
 * A backtrace is most useful when a program CRASHES. Run a crashing program
 * under the debugger and, when it stops with the error, type `backtrace`: the
 * top frame is the line that crashed, and the frames below it are how the
 * program got there.
 */

/*
 * --- Part 3: Catching the Hidden Bug ---
 *
 * Now build the program with the bug switched on:
 *
 *    cc -g -O0 -Wall -Wextra -std=c17 -DSHOW_BUG -o 36_debugging_bug 36_debugging.c
 *    ./36_debugging_bug
 *
 * Reading past the end of an array is UNDEFINED BEHAVIOR (Lesson 8). The total
 * and average may come out wrong, or they may look right by luck. You cannot
 * trust either result, and that is exactly what makes this kind of bug
 * dangerous: the program does not tell you anything is wrong.
 *
 * A SANITIZER is a checker the compiler builds into your program. ADDRESSSANITIZER
 * (ASan) checks every memory access while the program runs and stops it at the
 * first bad one. Turn it on with `-fsanitize=address`:
 *
 *    cc -g -O0 -Wall -Wextra -std=c17 -DSHOW_BUG -fsanitize=address -o 36_debugging_bug 36_debugging.c
 *    ./36_debugging_bug
 *
 * (ASan works with Clang and GCC on Linux and WSL, and with Clang on macOS.)
 * The program now stops with a report. Shortened, it looks like this; the
 * addresses and line numbers will differ on your machine:
 *
 *    ==12345==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffc...
 *    READ of size 4 at 0x7ffc... thread T0
 *        #0 0x... in sum_readings 36_debugging.c:<line>
 *        #1 0x... in main 36_debugging.c:<line>
 *    ...
 *      This frame has 1 object(s):
 *        [32, 60) 'readings' <== Memory access at offset 60 overflows this variable
 *
 * HOW TO READ IT:
 *
 * - The first line names the kind of error: a STACK-BUFFER-OVERFLOW is an
 *   access past the end of an array that lives on the stack (a local
 *   variable).
 * - "READ of size 4" says the program READ 4 bytes, one `int`.
 * - The numbered lines are a BACKTRACE, exactly like the debugger's: frame #0
 *   is where the bad read happened, `sum_readings`, with its file and line
 *   (thanks to `-g`), and frame #1 is `main`, which called it.
 * - The last part names the variable: the read went just past the end of
 *   `readings`.
 *
 * Always read a sanitizer report from the top, and look at frame #0 first.
 * Lesson 37 meets ASan's partner, UndefinedBehaviorSanitizer.
 */

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  Build with `-g -O0` when you debug: `-g` adds DEBUG INFORMATION and `-O0`
 *     keeps the program in the order you wrote it.
 * 2.  A DEBUGGER (gdb or lldb) can stop at a BREAKPOINT, step line by line,
 *     print variables and show the BACKTRACE of calls that led to a line.
 * 3.  When a program crashes, run it in the debugger and type `backtrace`:
 *     frame #0 is the line that crashed.
 * 4.  A memory bug may produce no visible symptom. `-fsanitize=address`
 *     (ASAN) stops the program at the first bad access and reports where it
 *     happened and which variable it hit.
 * 5.  Read sanitizer reports from the top: the error kind, then frame #0.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal. On Windows, use the Ubuntu terminal of WSL.
 * 2. Navigate to the directory where you saved this file.
 * 3. Compile it with debug information:
 *    `gcc -g -O0 -Wall -Wextra -std=c17 -o 36_debugging 36_debugging.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./36_debugging`
 *    - On Windows (WSL): `./36_debugging`
 * 5. Debug it: `gdb ./36_debugging` on Linux and WSL, `lldb ./36_debugging` on macOS.
 */
```

## How to Compile and Run

```sh
cc -Wall -Wextra -std=c17 -o 36_debugging 36_debugging.c
./36_debugging
```

## Try It Yourself

Practise what this lesson taught. Change a copy of the lesson file, or
start a new one.

1. Set a breakpoint in `highest_reading` and use `print highest` (or `p highest` in lldb) after each pass of its loop. On which pass does it change to 24?
2. Use a watchpoint on `total` in `sum_readings` and `continue` until it stops seven times. What values does `total` take?
3. Switch on the bug with `-DSHOW_BUG` and build it without a sanitizer. Run it a few times: is the total wrong? Then build it with `-fsanitize=address` and find the line number of the bad read in the report.
