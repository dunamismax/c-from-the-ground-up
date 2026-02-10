/**
 * @file 33_advanced_terminal_ui.c
 * @brief Part 5, Lesson 33: Advanced Terminal UI with ncurses
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for creating a rich
 * Terminal User Interface (TUI) using the ncurses library.
 * It explains the core concepts through structured comments and
 * provides a runnable example of their implementation.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, our programs have been linear: we `printf` some text, `scanf` some
 * input, and the text scrolls up and away forever. It's functional, but not
 * very interactive. Today, we take control of the entire terminal screen.
 *
 * WHAT IS NCURSES?
 * NCURSES is a powerful external library that allows you to create complex
 * Terminal User Interfaces (TUIs). It gives you precise control over:
 *  - Cursor position: Move the cursor anywhere on the screen.
 *  - Text attributes: Display text in bold, underlined, or in color.
 *  - Windows: Divide the screen into independent sections.
 *  - Input: Read single keystrokes (like arrow keys) without waiting for Enter.
 *
 * NCURSES is the foundation for many classic terminal applications, like the
 * text editor `vim`, the system monitor `htop`, and many more. This lesson
 * will introduce you to the basic building blocks.
 *
 * IMPORTANT: This program will take over your terminal screen. It will look
 * different from our previous programs!
 */

// To use ncurses, you must include its header file.
#include <ncurses.h>
#include <unistd.h> // We'll use sleep() for a small delay.

// --- Function Prototypes ---
// Good practice to declare our functions before main.
void initialize_ncurses(void);
void cleanup_ncurses(void);
void draw_ui(void);

// --- The Main Function ---

int main(void)
{
    initialize_ncurses();

    draw_ui();

    // After drawing, we wait for the user to press any key before exiting.
    // getch() is the ncurses function to get a single character of input.
    getch();

    cleanup_ncurses();

    return 0;
}

// --- Part 1: Initialization and Cleanup ---
/*
 * Every ncurses program needs a standard setup and cleanup routine.
 */
void initialize_ncurses(void)
{
    // initscr() is the FIRST ncurses function you must call. It determines the
    // terminal type, allocates memory, and effectively "takes over" the screen.
    initscr();

    // noecho() prevents keys pressed by the user from being automatically
    // displayed on the screen. This gives us full control over what appears.
    noecho();

    // cbreak() makes input available to the program immediately, without
    // requiring the user to press Enter.
    cbreak();

    // keypad() enables the reading of function keys like F1, F2, and arrow keys.
    // `stdscr` is the default WINDOW that represents the entire screen.
    keypad(stdscr, TRUE);

    // --- Color Initialization ---
    // To use colors, we must check if the terminal supports them.
    if (has_colors() == FALSE)
    {
        // If not, we have to exit gracefully.
        endwin(); // This restores the terminal to its normal state.
        printf("Your terminal does not support color\n");
        // exit(1); // We don't have this yet, so we'll let main return.
    }

    // If colors are supported, we must start the color system.
    start_color();

    // Now we can define COLOR PAIRS. A pair is a foreground/background combination.
    // We give each pair a number. Pair 0 is reserved for the default terminal colors.
    // init_pair(pair_number, foreground_color, background_color);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Yellow text on a black background
    init_pair(2, COLOR_CYAN, COLOR_BLACK);   // Cyan text on a black background
    init_pair(3, COLOR_WHITE, COLOR_BLUE);   // White text on a blue background
}

/*
 * The `endwin()` function is CRITICAL. It must be called before your program
 * exits to restore the terminal to its normal, sane state. If you forget this,
 * your terminal prompt might be broken!
 */
void cleanup_ncurses(void)
{
    endwin();
}

// --- Part 2: Drawing to the Screen ---
/*
 * NCURSES uses a VIRTUAL SCREEN. When you "print" something, it's drawn to an
 * in-memory representation of the screen first. Nothing appears on the actual
 * terminal until you call `refresh()`.
 */
void draw_ui(void)
{
    // Clear the screen
    clear();

    // --- Basic Drawing on `stdscr` ---
    // `mvprintw(row, col, "text")` moves the cursor to (row, col) and prints.
    // (0, 0) is the top-left corner.
    mvprintw(0, 0, "Welcome to the NCURSES Interactive Environment!");

    // Let's use our color pairs. `attron` turns an attribute ON.
    attron(COLOR_PAIR(1) | A_BOLD); // Turn on Yellow (Pair 1) and BOLD text.
    mvprintw(2, 2, "This text is Yellow and Bold!");
    attroff(COLOR_PAIR(1) | A_BOLD); // Turn the attributes OFF.

    mvprintw(4, 2, "Normal text again.");

    // The REFRESH command. This flushes the virtual screen buffer and updates
    // the physical terminal to show what we've drawn.
    refresh();

    sleep(2); // Pause for 2 seconds to let the user see the first part.

    // --- Creating a WINDOW ---
    // A WINDOW is a rectangular subsection of the screen that you can draw on
    // independently. This is key for organized UIs.

    int height = 10, width = 60;
    int start_y = 6, start_x = 5;

    // `newwin()` creates a new window.
    WINDOW *info_win = newwin(height, width, start_y, start_x);

    // `box(win, 0, 0)` draws a border around the window using default characters.
    box(info_win, 0, 0);

    // Let's draw inside our new window.
    // We'll use the window-specific version of printw: `wprintw`.
    // But first, let's change the window's color scheme.
    wbkgd(info_win, COLOR_PAIR(3)); // Set background for the window

    // Note: coordinates for drawing are relative to the WINDOW's top-left corner.
    // (1, 1) here is one character down and one right from the window's border.
    mvwprintw(info_win, 1, 1, "This is a new window!");

    attron(COLOR_PAIR(2)); // Use a different color for this next part
    mvwprintw(info_win, 3, 3, "You can draw here without affecting the main screen.");
    mvwprintw(info_win, 5, 3, "Press any key to exit this application.");
    attroff(COLOR_PAIR(2));

    // To display the window, we must refresh IT.
    wrefresh(info_win);
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * Just like we used `-lm` for the math library, we need to tell the linker to
 * include the ncurses library. The library's name is `ncurses`.
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler, adding the `-lncurses` flag AT THE END:
 *
 *    `gcc -Wall -Wextra -std=c11 -o 33_advanced_terminal_ui 33_advanced_terminal_ui.c -lncurses`
 *
 * THE -lncurses FLAG
 * This is the crucial part. `-l` tells the linker to look for a library, and
 * `ncurses` is the name of the library we want. Forgetting this flag will
 * result in "undefined reference" errors for all the ncurses functions like
 * `initscr`, `mvprintw`, etc.
 *
 * 4. Run the executable:
 *    - On Linux/macOS:   `./33_advanced_terminal_ui`
 *    - On Windows:       `33_advanced_terminal_ui.exe`
 *
 *    Your terminal will clear and show the UI we just created! Press any
 *    key to return to your normal command prompt.
 */