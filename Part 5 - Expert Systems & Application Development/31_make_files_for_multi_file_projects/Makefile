# Makefile
# @brief Part 5, Lesson 31: Makefiles for Multi-File Projects
# @author dunamismax
# @date 06-15-2025
#
# This file is the lesson on Makefiles. Read the comments from top to bottom
# to learn how to automate the compilation of complex C projects.

# =====================================================================================
# |                                   - LESSON START -                                  |
# =====================================================================================
#
# Welcome to a pivotal moment in your C journey! Until now, we've worked with
# single source files. While great for learning, real-world applications are almost
# always split into multiple files for organization, reusability, and clarity.
#
# Typing out long `gcc` commands for every file becomes tedious and error-prone.
# Imagine a project with 50 files! This is where the `make` utility comes in.
#
# WHAT IS A MAKEFILE?
# A `make` program reads a special file, by default named `Makefile`, to understand
# how to build your project. A Makefile contains a set of RULES. Each rule has
# three parts: a TARGET, its DEPENDENCIES, and a RECIPE.
#
#   target: dependencies
#       <tab>recipe (the command to run)
#
# - TARGET: The file we want to build (e.g., an executable or an object file).
# - DEPENDENCIES: The files the target needs in order to be built.
# - RECIPE: The command(s) to execute to create the target.
#
# IMPORTANT: The lines with recipes MUST start with a TAB character, not spaces!

# --- Part 1: Using Variables ---
#
# To make our Makefile clean and easy to modify, we use VARIABLES.
# It's convention to use all caps for variable names.

# The C compiler we want to use.
CC = gcc

# The flags we pass to the compiler. These are the same flags we've been using.
# CFLAGS stands for "C Compiler Flags".
CFLAGS = -Wall -Wextra -std=c11

# The name of the final executable file we want to build.
TARGET = 31_project_main

# --- Part 2: Defining the Rules ---
#
# Now we define the rules for building our project. `make` reads these rules
# and figures out what needs to be compiled or re-compiled.

# The first rule in a Makefile is the default rule. When you just type `make`
# in the terminal, this is the target that `make` will try to build. We want
# it to build our entire project.
#
# This rule says: To make the TARGET (`31_project_main`), you first need
# `main.o` and `helper.o`.
# The recipe then links these two object files together to create the final executable.
$(TARGET): main.o helper.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o helper.o

# This rule says: To make `main.o`, you need `main.c` and `helper.h`.
# If `main.c` or `helper.h` has changed since the last time `main.o` was built,
# `make` will run the recipe. The `-c` flag tells GCC to COMPILE ONLY, creating
# an "object file" (`.o`) without linking.
main.o: main.c helper.h
	$(CC) $(CFLAGS) -c main.c

# This is the rule for our helper module.
# To make `helper.o`, you need `helper.c` and `helper.h`.
helper.o: helper.c helper.h
	$(CC) $(CFLAGS) -c helper.c

# --- Part 3: Phony Targets ---
#
# Sometimes we want a target that isn't an actual file. For example, a "clean"
# target to delete all the compiled files. These are called PHONY targets.
# We declare them with `.PHONY` to tell `make` that this target name doesn't
# correspond to a file it should expect to exist.

.PHONY: all clean

# A common convention is to have an `all` target that is just another name for
# the default target. It doesn't do anything new but provides a clear command.
all: $(TARGET)

# This rule cleans up our directory.
# The `rm` command removes files. The `-f` flag means "force", so it won't
# complain if the files don't exist.
# We remove the object files (`*.o`) and the final executable (`$(TARGET)`).
clean:
	rm -f $(TARGET) *.o

# =====================================================================================
# |                                    - LESSON END -                                   |
# =====================================================================================
#
# HOW TO USE THIS MAKEFILE:
#
# 1. Open a terminal in the directory containing this Makefile and the .c/.h files.
#
# 2. To build the entire project:
#    `make`
#    (or `make all`)
#
#    You will see the commands from the recipes being executed. An executable
#    named `31_project_main` will be created.
#
# 3. To run the program:
#    - On Linux/macOS:   `./31_project_main`
#    - On Windows:       `31_project_main.exe`
#
# 4. To clean up all generated files (the executable and object files):
#    `make clean`
#
# Try it! Run `make`, then run the program. Then run `make` again. Notice how
# `make` says everything is "up to date"? Now, modify `helper.c` (e.g., add a
# comment) and run `make` again. See how it only re-compiles what's necessary?
# This is the power of `make`!