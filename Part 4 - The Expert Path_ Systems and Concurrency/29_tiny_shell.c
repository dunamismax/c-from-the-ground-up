/**
 * @file 29_tiny_shell.c
 * @brief Part 4, Lesson 29: Build Your Own Tiny Shell
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file implements a basic command-line shell, a program that reads
 * commands from the user and executes them. This is a capstone project for
 * understanding process management in a Unix-like environment.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * PROJECT: BUILD A TINY SHELL
 *
 * A SHELL is a command-line interpreter. It's the program that gives you a
 * prompt (like `$` or `> `), reads your input (like `ls -l`), and tells the
 * operating system to execute that command. In this lesson, we will build one.
 *
 * This project is the culmination of many systems programming concepts. It's how
 * a computer can run a program from within another program. The core of any
 * shell revolves around a simple, powerful loop:
 *
 * 1. READ: Read a command from the user.
 * 2. PARSE: Break the command into a program name and its arguments.
 * 3. FORK: Create a new PROCESS (a copy of the shell).
 * 4. EXEC: The new process REPLACES itself with the command to be run.
 * 5. WAIT: The original shell process waits for the command to finish.
 *
 * THE KEY SYSTEM CALLS
 * To accomplish this, we will use three of the most important functions in
 * Unix/Linux programming:
 *
 * - fork(): This creates a new child process, which is an exact duplicate of
 *   the parent process (our shell). It's like cloning our program. The parent
 *   gets the child's process ID, and the child gets 0.
 *
 * - execvp(): (The "exec" family of functions). This function REPLACES the
 *   current process's memory space with a new program. When the child process
 *   calls `execvp`, it ceases to be a copy of our shell and BECOMES the `ls`
 *   program (or whatever was requested). `execvp` is special because it
 *   searches the system's PATH for the executable, just like a real shell.
 *
 * - waitpid(): This tells the parent process (our shell) to pause and wait
 *   until a specific child process has finished executing. This ensures our
 *   shell prompt doesn't reappear until the command's output is complete.
 *
 * Let's build our own `bash`!
 */

// --- Required Headers ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // For strtok()
#include <unistd.h>   // For fork(), execvp()
#include <sys/wait.h> // For waitpid()

// --- Constants ---
#define MAX_LINE 80 // The maximum length of a command
#define MAX_ARGS 20 // The maximum number of arguments

// --- Function to Parse Input ---
// This function takes a raw command line string and breaks it into tokens.
// It populates the `args` array and is a crucial pre-processing step.
void parse_input(char *line, char **args)
{
    int i = 0;
    // `strtok` splits a string by a delimiter. We use space, tab, and newline.
    // The first call to `strtok` uses the line; subsequent calls use NULL
    // to continue tokenizing the same string.
    char *token = strtok(line, " \t\n");

    while (token != NULL && i < MAX_ARGS - 1)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\n");
    }
    // The `execvp` function requires the argument array to be terminated by a NULL pointer.
    args[i] = NULL;
}

int main(void)
{
    char *args[MAX_ARGS]; // Array to hold parsed command arguments
    char line[MAX_LINE];  // Buffer to hold the raw input line
    int should_run = 1;   // Flag to control the main loop

    printf("Welcome to TinyShell! Type 'exit' to quit.\n");

    while (should_run)
    {
        printf("> ");
        // We must flush stdout to ensure the prompt `>` appears before `fgets` waits for input.
        fflush(stdout);

        // --- Step 1: Read Input ---
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            // If fgets returns NULL, it's an end-of-file (Ctrl+D) or an error.
            printf("\nExiting TinyShell.\n");
            break;
        }

        // --- Step 2: Parse Input ---
        parse_input(line, args);

        // If the user just hits Enter, args[0] will be NULL. Loop again.
        if (args[0] == NULL)
        {
            continue;
        }

        // --- Handle Built-in Commands ---
        // We handle `exit` ourselves; it's not an external program.
        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            continue;
        }

        // --- Step 3: Fork a Child Process ---
        pid_t pid = fork();

        if (pid < 0)
        {
            // Forking failed. This is a serious error.
            perror("fork failed");
            exit(1);
        }
        else if (pid == 0)
        {
            // --- This is the CHILD PROCESS ---
            // --- Step 4: Execute the Command ---

            // `execvp` takes the program name (args[0]) and the full argument vector.
            // If it is successful, it NEVER returns. The child process becomes `ls` (or whatever).
            if (execvp(args[0], args) == -1)
            {
                // If `execvp` returns, an error occurred (e.g., command not found).
                perror("execvp failed");
                exit(1); // IMPORTANT: Terminate the child process on failure.
            }
        }
        else
        {
            // --- This is the PARENT PROCESS ---
            // --- Step 5: Wait for the Child to Complete ---
            int status;
            // `waitpid` waits for the process with the given `pid` to finish.
            // The `&status` argument can be used to get info on how the child exited.
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * You have successfully built a functional command-line shell! It can run almost
 * any standard command-line program on your system. You have directly manipulated
 * processes, the fundamental unit of execution in an operating system.
 *
 * This Read-Parse-Fork-Exec-Wait loop is the foundation of not just shells, but
 * also IDE "Run" buttons, web servers that spawn worker processes, and many other
 * powerful system tools.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal and compile the program:
 *    `gcc -Wall -Wextra -std=c11 -o 29_tiny_shell 29_tiny_shell.c`
 *
 * 2. Run your new shell:
 *    `./29_tiny_shell`
 *
 * 3. You will see a `>` prompt. Try running some commands!
 *    > ls
 *    > ls -l
 *    > pwd
 *    > echo "Hello from my own shell!"
 *    > whoami
 *
 * 4. When you are finished, type `exit` to quit your shell and return to the
 *    regular system shell.
 *    > exit
 */