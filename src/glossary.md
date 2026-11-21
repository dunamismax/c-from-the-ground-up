# Glossary

The key terms the lessons introduce, in the UPPERCASE the lesson comments
use when a term first appears. Each entry links to the lesson that teaches it.

## A

- **Address**: The number that identifies a location in memory. Every variable lives at an address, which `&` gives you and `%p` prints. ([Lesson 10](chapters/10-pointers.md))
- **Address-of operator (`&`)**: Gives the memory address of a variable, as in `&user_age`. `scanf` needs it so it knows where to store what it reads. ([Lesson 3](chapters/03-user-input.md))
- **AddressSanitizer (ASan)**: A sanitizer, turned on with `-fsanitize=address`, that stops a program at its first out-of-bounds access, use after `free` or similar memory error and reports where it happened. ([Lesson 36](chapters/36-debugging.md))
- **`argc` and `argv`**: The two parameters of `main` that receive command-line arguments: the argument count, and an array of the arguments as strings. `argv[0]` is the program's name. ([Lesson 15](chapters/15-command-line-arguments.md))
- **Argument**: A value passed to a function when it is called. In `calculate_damage(20, 8)`, 20 and 8 are the arguments. ([Lesson 7](chapters/07-functions.md))
- **Array**: A fixed-size, numbered sequence of elements of the same type, stored next to each other in memory. ([Lesson 8](chapters/08-arrays.md))
- **Arrow operator (`->`)**: Reaches a struct member through a pointer: `p->gpa` means `(*p).gpa`. ([Lesson 12](chapters/12-structs.md))
- **Assertion (`assert`)**: A check from `<assert.h>` that stops the program if a condition that must be true is false. Compiling with `-DNDEBUG` removes it. ([Lesson 38](chapters/38-testing.md))
- **Atomic operation**: An operation that happens as one indivisible step, so no other thread can see it half done. `count++` is not atomic. ([Lesson 30](chapters/30-multithreaded-file-analyzer.md))

## B

- **Backtrace**: The chain of function calls that led to the current line, innermost first. Also called the call stack. ([Lesson 36](chapters/36-debugging.md))
- **Base case**: The condition that stops a recursive function from calling itself again. Without one, recursion never ends. ([Lesson 19](chapters/19-recursion.md))
- **Binary library file**: The compiled code of a library (`.so` on Linux, `.dylib` on macOS) that the linker connects to your program. ([Lesson 32](chapters/32-linking-external-libraries.md))
- **Bit**: A single binary digit, 0 or 1. A byte is 8 bits. ([Lesson 21](chapters/21-bit-manipulation.md))
- **Bitmask**: A value whose bits select other bits, used with `&`, `|` and `^` to set, clear, test or toggle flags. ([Lesson 21](chapters/21-bit-manipulation.md))
- **Bitwise operators**: Operators that work on the individual bits of integers: `&`, `|`, `^`, `~`, `<<` and `>>`. ([Lesson 21](chapters/21-bit-manipulation.md))
- **Blocking call**: A function that waits until something happens, such as `accept()` waiting for a client to connect. ([Lesson 26](chapters/26-simple-sockets.md))
- **Breakpoint**: A place where a debugger pauses the program, such as the start of a function. ([Lesson 36](chapters/36-debugging.md))
- **Buffer**: A block of memory that holds data on its way somewhere, such as a `char` array that `fgets` reads a line into. ([Lesson 6](chapters/06-loops.md))
- **Buffer overflow**: Writing past the end of a buffer. It corrupts other memory and is a classic source of crashes and security holes. ([Lesson 9](chapters/09-strings.md))

## C

- **Callback**: A function passed (as a function pointer) to another function, which calls it to do part of its work. ([Lesson 18](chapters/18-function-pointers.md))
- **Cast**: An explicit conversion to another type, written `(type)value`, as in `(double)a / b`. ([Lesson 13](chapters/13-dynamic-memory-allocation.md))
- **Character literal**: A single character in single quotes, such as `'A'`. Its type is `int`, and it fits in a `char`. ([Lesson 2](chapters/02-variables-and-data-types.md))
- **Child process**: The new process that `fork()` creates. It starts as a copy of its parent. ([Lesson 29](chapters/29-tiny-shell.md))
- **Client-server model**: A design where a server waits for requests and clients connect to it to make them. ([Lesson 26](chapters/26-simple-sockets.md))
- **Collision**: Two different keys hashing to the same bucket of a hash table. ([Lesson 28](chapters/28-hash-table.md))
- **Compiler**: The program that translates C source code into machine code your computer can run. In this course it is `cc`: GCC or Clang. ([Lesson 1](chapters/01-hello-world.md))
- **Conditional compilation**: Using `#if`, `#ifdef` and `#ifndef` to include or leave out code before compiling, for example debug output. ([Lesson 22](chapters/22-preprocessor-directives.md))
- **Conditional statement**: Code that runs only when a condition is true: `if`, `else if`, `else` and `switch`. ([Lesson 5](chapters/05-conditional-statements.md))
- **Critical section**: Code that touches shared data and must be run by only one thread at a time, usually guarded by a mutex. ([Lesson 30](chapters/30-multithreaded-file-analyzer.md))

## D

- **Dangling pointer**: A pointer to memory that has been freed. Using it is undefined behavior, which is why lessons set pointers to `NULL` after `free`. ([Lesson 13](chapters/13-dynamic-memory-allocation.md))
- **Data type**: What kind of value a variable holds, and so how much memory it needs and what you can do with it: `int`, `double`, `char` and so on. ([Lesson 2](chapters/02-variables-and-data-types.md))
- **Debug information**: The map from machine code back to source files, lines and variable names that `-g` adds, so a debugger and sanitizers can name them. ([Lesson 36](chapters/36-debugging.md))
- **Debugger**: A program, such as gdb or lldb, that runs your program under its control so you can pause it, step through it and inspect its variables. ([Lesson 36](chapters/36-debugging.md))
- **Declaration**: Introducing a name and its type to the compiler, as in `int player_score;`. A function declaration is its prototype. ([Lesson 2](chapters/02-variables-and-data-types.md))
- **Definition**: The part that actually creates something: a function with its body, or a variable with its storage. ([Lesson 7](chapters/07-functions.md))
- **Dereference**: Following a pointer to the value it points to, with `*`: `*p_score` is the `int` that `p_score` points at. ([Lesson 10](chapters/10-pointers.md))
- **Doubly linked list**: A linked list whose nodes point both to the next node and to the previous one. ([Lesson 25](chapters/25-simple-text-editor.md))
- **Dynamic memory allocation**: Requesting memory while the program runs, with `malloc` (and `realloc`), and giving it back with `free`. ([Lesson 13](chapters/13-dynamic-memory-allocation.md))

## E

- **Edge case**: An input at the boundary of what code handles, such as an empty string or a value exactly at a limit. Bugs hide there, so tests should cover them. ([Lesson 38](chapters/38-testing.md))
- **Enum**: A type made of named integer constants, such as `SUCCESS` and `FAILURE`, so code uses names instead of magic numbers. ([Lesson 23](chapters/23-unions-and-enums.md))
- **EOF**: "End of file": the special negative value input functions return when there is no more input. Ctrl+D sends it from a terminal. ([Lesson 3](chapters/03-user-input.md))
- **Escape sequence**: A backslash code inside a string or character literal for a character you cannot type directly, such as `\n` (newline). ([Lesson 1](chapters/01-hello-world.md))
- **Exit status**: The number `main` returns to the operating system: 0 for success, anything else for failure. Scripts and CI read it. ([Lesson 38](chapters/38-testing.md))
- **`extern`**: Declares a global variable that is defined in another source file, so this file can use it. ([Lesson 24](chapters/24-static-and-extern-variables.md))

## F

- **File descriptor**: A small integer the operating system uses to identify an open file or socket. ([Lesson 26](chapters/26-simple-sockets.md))
- **`FILE` pointer**: The `FILE *` that `fopen` returns and every stdio file function takes. Always check it for `NULL`. ([Lesson 14](chapters/14-file-io.md))
- **Floating-point**: Numbers with a fractional part, stored as `float` or `double`. This course prefers `double`. ([Lesson 2](chapters/02-variables-and-data-types.md))
- **`fork()`**: The POSIX system call that creates a new process. It returns 0 in the child and the child's ID in the parent. ([Lesson 29](chapters/29-tiny-shell.md))
- **Format specifier**: A placeholder such as `%d`, `%f`, `%c` or `%s` that tells `printf` or `scanf` what type of value goes there. ([Lesson 2](chapters/02-variables-and-data-types.md))
- **Function**: A named block of code that does one task, can take parameters and can return a value. ([Lesson 7](chapters/07-functions.md))
- **Function pointer**: A variable that holds the address of a function, so the function to call can be chosen at run time. ([Lesson 18](chapters/18-function-pointers.md))

## H

- **Hash function**: A function that turns a key into an array index for a hash table. ([Lesson 28](chapters/28-hash-table.md))
- **Hash table**: A data structure that maps keys to values, using a hash function to find each key's bucket quickly. ([Lesson 28](chapters/28-hash-table.md))
- **Header file**: A `.h` file of declarations (function prototypes, types, macros) that other files `#include`. ([Lesson 31](chapters/31-makefiles.md))
- **Header guard**: The `#ifndef NAME` / `#define NAME` / `#endif` wrapper that stops a header from being included twice. Also called an include guard. ([Lesson 22](chapters/22-preprocessor-directives.md))
- **Heap**: The pool of memory that `malloc` hands out. Memory there lives until you `free` it. ([Lesson 13](chapters/13-dynamic-memory-allocation.md))

## I

- **Index**: The position of an element in an array. C counts from 0. ([Lesson 8](chapters/08-arrays.md))
- **Infinite loop**: A loop whose condition never becomes false, so it never ends. ([Lesson 6](chapters/06-loops.md))
- **Initialization**: Giving a variable its first value, ideally when you declare it: `int number_of_lives = 3;`. ([Lesson 2](chapters/02-variables-and-data-types.md))
- **Input buffer**: Where typed input waits until a program reads it. Characters `scanf` does not use, such as a newline, stay there for the next read. ([Lesson 3](chapters/03-user-input.md))
- **Integer division**: Dividing two integers in C, which throws away the fractional part: `10 / 3` is 3. ([Lesson 4](chapters/04-basic-operators.md))
- **Integer promotion**: C widening small types such as `char` or `uint8_t` to `int` before doing arithmetic or bitwise operations on them. ([Lesson 21](chapters/21-bit-manipulation.md))
- **Iteration**: One run through the body of a loop. ([Lesson 6](chapters/06-loops.md))

## L

- **Linkage**: Whether a name can be seen from other source files. `static` at file scope gives internal linkage: private to its file. ([Lesson 24](chapters/24-static-and-extern-variables.md))
- **Linked list**: A chain of nodes, each holding data and a pointer to the next node. It can grow and shrink one node at a time. ([Lesson 20](chapters/20-linked-lists.md))
- **Linker**: The tool that joins object files and libraries into one program, connecting each function call to its definition. ([Lesson 31](chapters/31-makefiles.md))
- **Logical operators**: `&&` (and), `||` (or) and `!` (not), which combine or invert true/false values. ([Lesson 4](chapters/04-basic-operators.md))

## M

- **Macro**: A name that the preprocessor replaces with text before compiling, made with `#define`. ([Lesson 22](chapters/22-preprocessor-directives.md))
- **Main function**: `main`, where every C program starts running. Its return value is the program's exit code. ([Lesson 1](chapters/01-hello-world.md))
- **Makefile**: The file `make` reads: rules saying how to build each target from its dependencies. ([Lesson 31](chapters/31-makefiles.md))
- **Member**: One of the named fields inside a struct, reached with `.` or `->`. ([Lesson 12](chapters/12-structs.md))
- **Memory leak**: Memory that was allocated and never freed. A long-running program that leaks keeps growing. ([Lesson 13](chapters/13-dynamic-memory-allocation.md))
- **Modulus (`%`)**: The remainder of an integer division: `10 % 3` is 1. ([Lesson 4](chapters/04-basic-operators.md))
- **Mutex**: A lock that only one thread can hold at a time, used to protect shared data from race conditions. ([Lesson 30](chapters/30-multithreaded-file-analyzer.md))

## N

- **ncurses**: A library for full-screen terminal programs: cursor movement, colors, windows and single-key input. ([Lesson 33](chapters/33-advanced-terminal-ui.md))
- **Network byte order**: The byte order (big-endian) that network protocols use. `htons` converts a port number to it. ([Lesson 26](chapters/26-simple-sockets.md))
- **Node**: One element of a linked list or tree: some data plus one or more pointers to other nodes. ([Lesson 20](chapters/20-linked-lists.md))
- **`NULL`**: A pointer value that points nowhere. Functions such as `malloc` and `fopen` return it to report failure. ([Lesson 10](chapters/10-pointers.md))
- **Null terminator**: The `'\0'` character that marks the end of a C string. ([Lesson 9](chapters/09-strings.md))

## O

- **Off-by-one error**: A loop or index that goes one step too far or stops one step short, such as using `<=` where `<` was needed. ([Lesson 36](chapters/36-debugging.md))
- **Operator precedence**: The rules for which operator is applied first, as in math: `5 + 2 * 10` is 25. Parentheses override them. ([Lesson 4](chapters/04-basic-operators.md))

## P

- **Parameter**: A variable in a function's definition that receives the value of an argument. ([Lesson 7](chapters/07-functions.md))
- **Parent process**: The process that called `fork()`. It usually waits for its child with `waitpid()`. ([Lesson 29](chapters/29-tiny-shell.md))
- **Parsing**: Turning text, such as a line of a data file, into structured values like the members of a struct. ([Lesson 34](chapters/34-parsing-data-files.md))
- **Pointer**: A variable that holds a memory address. `int *p` is a pointer to an `int`. ([Lesson 10](chapters/10-pointers.md))
- **Pointer arithmetic**: Adding to or subtracting from a pointer to move between array elements: `p + 1` points at the next element. ([Lesson 11](chapters/11-pointers-and-arrays.md))
- **Port**: A number that, with an IP address, identifies one program's network endpoint on a machine. ([Lesson 26](chapters/26-simple-sockets.md))
- **POSIX**: The standard for Unix-like operating systems (Linux, macOS) that defines APIs such as `fork`, sockets and pthreads. Lessons 26-30 use it. ([Lesson 30](chapters/30-multithreaded-file-analyzer.md))
- **Preprocessor**: The first stage of compiling, which handles lines starting with `#`: includes, macros and conditional compilation. ([Lesson 22](chapters/22-preprocessor-directives.md))
- **Preprocessor directive**: A line starting with `#`, such as `#include <stdio.h>`, handled by the preprocessor before compiling. ([Lesson 1](chapters/01-hello-world.md))
- **Process**: A running program, with its own memory. `fork()` creates a new one. ([Lesson 29](chapters/29-tiny-shell.md))
- **Prototype**: A function's declaration: its return type, name and parameter types, ending in a semicolon. It lets you call a function defined later. ([Lesson 7](chapters/07-functions.md))

## R

- **Race condition**: A bug where the result depends on how threads happen to interleave, such as two threads both updating a counter and one update being lost. ([Lesson 30](chapters/30-multithreaded-file-analyzer.md))
- **`realloc`**: Resizes a block from `malloc`, copying its contents if it has to move. On failure it returns `NULL` and leaves the old block alone. ([Lesson 25](chapters/25-simple-text-editor.md))
- **Recursion**: A function calling itself on a smaller version of its problem until it reaches a base case. ([Lesson 19](chapters/19-recursion.md))
- **Relational operators**: `==`, `!=`, `<`, `>`, `<=` and `>=`, which compare two values and give 1 (true) or 0 (false). ([Lesson 4](chapters/04-basic-operators.md))
- **Return value**: The value a function sends back to its caller with `return`. ([Lesson 7](chapters/07-functions.md))

## S

- **Sanitizer**: A checker the compiler builds into a program (`-fsanitize=...`) that reports certain bugs while it runs. ([Lesson 36](chapters/36-debugging.md))
- **Separate chaining**: Handling hash collisions by keeping a linked list of entries in each bucket. ([Lesson 28](chapters/28-hash-table.md))
- **Shell**: A program that reads commands and runs them, such as `bash` or `zsh`. ([Lesson 29](chapters/29-tiny-shell.md))
- **Signed integer overflow**: Going past the largest or smallest value of a signed type such as `int`. It is undefined behavior, unlike unsigned arithmetic, which wraps around. ([Lesson 37](chapters/37-undefined-behavior.md))
- **Socket**: An endpoint for network communication, used like a file descriptor with `send` and `recv`. ([Lesson 26](chapters/26-simple-sockets.md))
- **Stack**: The memory that holds a function's local variables. It is freed automatically when the function returns. ([Lesson 13](chapters/13-dynamic-memory-allocation.md))
- **Stack overflow**: Running out of stack memory, typically from recursion that never stops. ([Lesson 19](chapters/19-recursion.md))
- **Statement**: One instruction in C, usually ending with a semicolon. ([Lesson 1](chapters/01-hello-world.md))
- **`static`**: Inside a function, makes a local variable keep its value between calls. At file scope, makes a name private to its file. ([Lesson 24](chapters/24-static-and-extern-variables.md))
- **Strict aliasing**: The rule that an object may only be accessed through its own type (or as raw bytes). Use `memcpy` to copy an object's bits into another type. ([Lesson 37](chapters/37-undefined-behavior.md))
- **String**: In C, an array of `char` ending with a null terminator. ([Lesson 9](chapters/09-strings.md))
- **String literal**: Text in double quotes, such as `"Hello, World!\n"`. The compiler adds the null terminator. ([Lesson 1](chapters/01-hello-world.md))
- **Stringizing operator (`#`)**: In a macro, `#name` turns the argument into a string literal, so a test can print the check exactly as written. ([Lesson 38](chapters/38-testing.md))
- **Struct**: A user-defined type that groups related variables, possibly of different types, under one name. ([Lesson 12](chapters/12-structs.md))

## T

- **Test harness**: The code that runs tests and reports the results, such as lesson 38's `CHECK` macro and its counters. ([Lesson 38](chapters/38-testing.md))
- **Thread**: A path of execution inside a process. Threads of one process share its memory. ([Lesson 30](chapters/30-multithreaded-file-analyzer.md))

## U

- **Undefined behavior**: Code whose result the C standard does not define, such as reading past the end of an array. Anything can happen, including appearing to work. ([Lesson 8](chapters/08-arrays.md))
- **UndefinedBehaviorSanitizer (UBSan)**: A sanitizer, turned on with `-fsanitize=undefined`, that reports undefined behavior such as signed overflow and bad shifts while the program runs. ([Lesson 37](chapters/37-undefined-behavior.md))
- **Union**: A type whose members share the same memory, so it holds only one of them at a time. ([Lesson 23](chapters/23-unions-and-enums.md))
- **Unit test**: A test of one small piece of code, usually one function, on inputs whose correct results you already know. ([Lesson 38](chapters/38-testing.md))

## V

- **Variable**: A named piece of memory that holds a value of a given type. ([Lesson 2](chapters/02-variables-and-data-types.md))
- **Virtual screen**: The in-memory copy of the screen that ncurses draws into. `refresh()` copies it to the real terminal. ([Lesson 33](chapters/33-advanced-terminal-ui.md))

## W

- **Watchpoint**: A debugger setting that pauses the program whenever a chosen variable changes. ([Lesson 36](chapters/36-debugging.md))
