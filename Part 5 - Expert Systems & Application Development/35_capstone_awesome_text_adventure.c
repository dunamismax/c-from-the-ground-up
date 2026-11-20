/**
 * @file 35_capstone_awesome_text_adventure.c
 * @brief Part 5, Lesson 35: Capstone - Awesome Text Adventure
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file contains the source code for the final capstone lesson.
 * It pulls together many earlier topics into a single ncurses-based
 * text adventure that loads a map file at runtime.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Welcome to the final project. This is it.
 *
 * Every lesson, from `printf` to pointers, from `malloc` to Makefiles, has been
 * leading to this. You are about to build a complete, interactive application
 * from the ground up. This project doesn't introduce many new C features;
 * instead, it challenges you to synthesize everything you've already learned.
 *
 * CONCEPTS YOU WILL USE IN THIS PROJECT:
 * - Foundational Logic: Variables, `if`/`else`, `for`/`while` loops, functions.
 * - Data Structures: `struct`s to model the game world, `enum`s for clarity.
 * - Memory Management: `malloc` and `free` to dynamically create the world.
 * - Pointers: The entire game world is connected by POINTERS. This is the ultimate
 *   demonstration of their power.
 * - File I/O: The game world is loaded from an external data file (`world.map`).
 * - Parsing: The world data file is parsed with `strtol` (Lesson 15) and by
 *   walking a `char *` through each line. Lesson 34 used `sscanf`; here every
 *   field is checked by hand, so a malformed map is rejected with a clear error.
 * - Command-Line Arguments: The program takes the map file as an argument.
 * - External Libraries: We use the `ncurses` library for an advanced terminal UI.
 * - Build Tooling: Lesson 31 introduced Makefiles, but this capstone stays in
 *   one source file so you can build it directly with a compiler command.
 * - Advanced Techniques: We'll even use an array of structs with function
 *   pointers for a clean command handling system.
 *
 * You are no longer a beginner. You are a C developer. Let's build something awesome.
 */

// --- Standard and External Library Includes ---
#include <ctype.h>   // For tolower()
#include <errno.h>   // For robust numeric parsing
#include <limits.h>  // For INT_MIN and INT_MAX
#include <ncurses.h> // For the advanced Terminal User Interface (TUI)
#include <stdio.h>
#include <stdlib.h> // For malloc, free, exit
#include <string.h> // For string manipulation functions

// --- Game Constants and Enums ---
#define MAX_DIRECTIONS 4
#define MAX_ROOMS 100
#define MAX_LOG_MESSAGES 10
#define INPUT_BUFFER_SIZE 100
#define ROOM_DESCRIPTION_SIZE 512
#define WORLD_LINE_BUFFER_SIZE 1024

// Using an enum makes direction-related code much more readable and safe.
typedef enum
{
    NORTH,
    SOUTH,
    EAST,
    WEST
} Direction;

// --- Core Data Structures ---
// These structs are the blueprint for our entire game world.

// Forward declaration is needed because GameState and Room reference each other.
struct GameState;

typedef struct Room
{
    int id;
    char description[ROOM_DESCRIPTION_SIZE];
    // This is the key: an array of POINTERS to other Room structs.
    // This is how we form the "graph" of our world map.
    struct Room *exits[MAX_DIRECTIONS];
} Room;

typedef struct Player
{
    Room *current_room; // A pointer to the room the player is currently in.
} Player;

typedef struct
{
    char *command;
    // A function pointer! This lets us create a clean, data-driven command system.
    void (*handler)(struct GameState *game, char *argument);
} Command;

// The main struct to hold the entire state of our running game.
typedef struct GameState
{
    Player player;
    Room *all_rooms[MAX_ROOMS];
    int num_rooms;
    int game_should_close;

    // For the ncurses UI
    WINDOW *main_win;
    WINDOW *status_win;
    WINDOW *input_win;
    char *log_messages[MAX_LOG_MESSAGES];
    int log_count;
} GameState;

// --- Function Prototypes ---
// Grouping prototypes makes the code easier to navigate.

// Game Lifecycle
void game_loop(GameState *game);
void cleanup(GameState *game);

// World Loading & Parsing
GameState *load_world(const char *filename);
int parse_room(char *line, GameState *game);
int parse_link(char *line, GameState *game);
Room *find_room_by_id(GameState *game, int id);
const char *direction_to_string(Direction d);
int read_world_line(FILE *file, char *buffer, size_t size);
char *skip_whitespace(char *text);
int parse_int_token(char **cursor, int *value);
int has_only_trailing_whitespace(char *text);
char *duplicate_string(const char *text);

// Command Handling
void parse_and_execute_command(GameState *game, char *input);
void handle_quit(GameState *game, char *argument);
void handle_look(GameState *game, char *argument);
void handle_go(GameState *game, char *argument);
void handle_help(GameState *game, char *argument);

// UI Functions (ncurses)
int ui_init(GameState *game);
void ui_draw(GameState *game);
int ui_get_input(GameState *game, char *buffer);
void ui_log(GameState *game, const char *message);
void ui_cleanup(GameState *game);

// --- Parsing Helpers ---
// Small functions used by the world loader below.

int read_world_line(FILE *file, char *buffer, size_t size)
{
    size_t length;

    // `fgets` takes an `int` size; our line buffer is small enough to fit.
    if (fgets(buffer, (int)size, file) == NULL)
    {
        return 0;
    }

    length = strcspn(buffer, "\n");
    if (buffer[length] == '\n')
    {
        buffer[length] = '\0';
        return 1;
    }

    if (!feof(file))
    {
        int ch;

        while ((ch = fgetc(file)) != '\n' && ch != EOF)
        {
            // Discard the rest of an overlong line so the next read starts cleanly.
        }
        buffer[0] = '\0';
        return -1;
    }

    return 1;
}

char *skip_whitespace(char *text)
{
    while (isspace((unsigned char)*text))
    {
        text++;
    }

    return text;
}

int parse_int_token(char **cursor, int *value)
{
    char *endptr;
    long parsed;

    *cursor = skip_whitespace(*cursor);
    if (**cursor == '\0')
    {
        return 0;
    }

    errno = 0;
    parsed = strtol(*cursor, &endptr, 10);
    if (errno != 0 || endptr == *cursor || parsed < INT_MIN || parsed > INT_MAX)
    {
        return 0;
    }

    *value = (int)parsed;
    *cursor = endptr;
    return 1;
}

int has_only_trailing_whitespace(char *text)
{
    text = skip_whitespace(text);
    return *text == '\0';
}

char *duplicate_string(const char *text)
{
    size_t length = strlen(text) + 1;
    char *copy = malloc(length);

    if (!copy)
    {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

// --- Main Program Entry ---
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        // We use fprintf to stderr for error messages.
        fprintf(stderr, "Usage: %s <world_map_file>\n", argv[0]);
        return 1;
    }

    // --- Initialization ---
    // Load the world from the file specified on the command line.
    GameState *game = load_world(argv[1]);
    if (game == NULL)
    {
        // load_world will have already printed an error.
        return 1;
    }

    // Initialize the ncurses interface. It fails if the terminal is too small.
    if (!ui_init(game))
    {
        cleanup(game);
        return 1;
    }

    // Set the player's starting location.
    game->player.current_room = find_room_by_id(game, 0);
    if (game->player.current_room == NULL)
    {
        ui_cleanup(game);
        cleanup(game);
        fprintf(stderr, "Error: Map has no starting room (ID 0).\n");
        return 1;
    }

    // --- Main Game Loop ---
    ui_log(game, "Welcome to the Awesome Text Adventure! Type 'help' for commands.");
    handle_look(game, NULL); // Give the player their initial view.
    game_loop(game);

    // --- Cleanup ---
    // This is crucial for both ncurses and memory management.
    ui_cleanup(game);
    cleanup(game);

    printf("Thank you for playing!\n");
    return 0;
}

// =====================================================================================
// |                            - WORLD LOADING & PARSING -                            |
// =====================================================================================

/**
 * @brief Loads the entire game world from a data file.
 * This function demonstrates dynamic allocation, file I/O, and robust parsing.
 */
GameState *load_world(const char *filename)
{
    // Dynamically allocate the main GameState struct.
    GameState *game = malloc(sizeof(GameState));
    int line_number = 0;
    int line_status;
    if (!game)
        return NULL;
    // ALWAYS initialize allocated memory.
    memset(game, 0, sizeof(GameState));

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening world file");
        free(game);
        return NULL;
    }

    char line[WORLD_LINE_BUFFER_SIZE];

    // --- TWO-PASS LOADING ---
    // This is a common and robust technique for loading interconnected data.
    // Pass 1: Read all room definitions and allocate memory for them.
    while ((line_status = read_world_line(file, line, sizeof(line))) != 0)
    {
        line_number++;
        if (line_status < 0)
        {
            fprintf(stderr, "Error: world file line %d is too long.\n", line_number);
            fclose(file);
            cleanup(game);
            return NULL;
        }

        if (strncmp(line, "room", 4) == 0)
        {
            if (!parse_room(line, game))
            {
                fprintf(stderr, "Error parsing room definition on line %d.\n", line_number);
                fclose(file);
                cleanup(game);
                return NULL;
            }
        }
    }

    // Go back to the beginning of the file for the second pass. `rewind(file)`
    // does the same but cannot report failure; `fseek` returns non-zero when
    // it fails, as in Lesson 30, so we can stop instead of reading nothing.
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        perror("Error rewinding world file");
        fclose(file);
        cleanup(game);
        return NULL;
    }
    line_number = 0;

    // Pass 2: Read all link definitions and connect the already-created rooms.
    while ((line_status = read_world_line(file, line, sizeof(line))) != 0)
    {
        line_number++;
        if (line_status < 0)
        {
            fprintf(stderr, "Error: world file line %d is too long.\n", line_number);
            fclose(file);
            cleanup(game);
            return NULL;
        }

        if (strncmp(line, "link", 4) == 0)
        {
            if (!parse_link(line, game))
            {
                fprintf(stderr, "Error parsing link definition on line %d.\n", line_number);
                fclose(file);
                cleanup(game);
                return NULL;
            }
        }
    }

    fclose(file);
    return game;
}

/**
 * @brief Parses a 'room' line and creates a Room struct.
 */
int parse_room(char *line, GameState *game)
{
    char *cursor = line;
    if (game->num_rooms >= MAX_ROOMS)
        return 0; // Guard against overflow.

    int id;
    char desc[ROOM_DESCRIPTION_SIZE];
    size_t desc_length = 0;

    if (strncmp(cursor, "room", 4) != 0 || !isspace((unsigned char)cursor[4]))
    {
        return 0;
    }
    cursor += 4;

    if (!parse_int_token(&cursor, &id))
    {
        return 0;
    }

    cursor = skip_whitespace(cursor);
    if (*cursor != '"')
    {
        return 0;
    }
    cursor++;

    while (*cursor != '"' && *cursor != '\0')
    {
        if (desc_length >= sizeof(desc) - 1)
        {
            return 0;
        }

        desc[desc_length++] = *cursor;
        cursor++;
    }

    if (*cursor != '"')
    {
        return 0;
    }

    desc[desc_length] = '\0';
    cursor++;

    if (!has_only_trailing_whitespace(cursor) || find_room_by_id(game, id) != NULL)
    {
        return 0;
    }

    Room *new_room = malloc(sizeof(Room));
    if (!new_room)
        return 0;

    memset(new_room, 0, sizeof(Room)); // Initialize room exits to NULL.
    new_room->id = id;
    memcpy(new_room->description, desc, desc_length + 1);

    game->all_rooms[game->num_rooms++] = new_room;
    return 1;
}

/**
 * @brief Parses a 'link' line and connects two existing rooms.
 */
int parse_link(char *line, GameState *game)
{
    char *cursor = line;
    int from_id, to_id;
    char dir_str[10];
    size_t dir_length = 0;

    if (strncmp(cursor, "link", 4) != 0 || !isspace((unsigned char)cursor[4]))
    {
        return 0;
    }
    cursor += 4;

    if (!parse_int_token(&cursor, &from_id))
    {
        return 0;
    }

    cursor = skip_whitespace(cursor);
    if (*cursor == '\0')
    {
        return 0;
    }

    while (*cursor != '\0' && !isspace((unsigned char)*cursor))
    {
        if (dir_length >= sizeof(dir_str) - 1)
        {
            return 0;
        }

        dir_str[dir_length++] = *cursor;
        cursor++;
    }
    dir_str[dir_length] = '\0';

    if (dir_length == 0 || !parse_int_token(&cursor, &to_id) || !has_only_trailing_whitespace(cursor))
    {
        return 0;
    }

    Room *from_room = find_room_by_id(game, from_id);
    Room *to_room = find_room_by_id(game, to_id);

    if (!from_room || !to_room)
        return 0; // One of the rooms doesn't exist.

    Direction dir;
    if (strcmp(dir_str, "n") == 0)
        dir = NORTH;
    else if (strcmp(dir_str, "s") == 0)
        dir = SOUTH;
    else if (strcmp(dir_str, "e") == 0)
        dir = EAST;
    else if (strcmp(dir_str, "w") == 0)
        dir = WEST;
    else
        return 0; // Invalid direction.

    // This is the magic: we store a POINTER to the 'to' room in the 'from' room's exit array.
    from_room->exits[dir] = to_room;
    return 1;
}

/**
 * @brief A helper function to find a room pointer from its integer ID.
 */
Room *find_room_by_id(GameState *game, int id)
{
    for (int i = 0; i < game->num_rooms; i++)
    {
        if (game->all_rooms[i]->id == id)
        {
            return game->all_rooms[i];
        }
    }
    return NULL; // Not found
}

const char *direction_to_string(Direction d)
{
    switch (d)
    {
    case NORTH:
        return "north";
    case SOUTH:
        return "south";
    case EAST:
        return "east";
    case WEST:
        return "west";
    }
    return "unknown";
}

// =====================================================================================
// |                                - COMMAND HANDLING -                               |
// =====================================================================================

// This array of structs is our command table. It pairs a command string with
// the function pointer that handles it. This is a very clean, scalable design.
Command command_table[] = {
    {"quit", handle_quit},
    {"exit", handle_quit},
    {"look", handle_look},
    {"l", handle_look},
    {"go", handle_go},
    {"north", handle_go},
    {"south", handle_go},
    {"east", handle_go},
    {"west", handle_go},
    {"n", handle_go},
    {"s", handle_go},
    {"e", handle_go},
    {"w", handle_go},
    {"help", handle_help},
    {NULL, NULL} // Sentinel to mark the end of the array.
};

/**
 * @brief Processes the user's raw input string, finds the correct command handler,
 *        and calls it.
 */
void parse_and_execute_command(GameState *game, char *input)
{
    char *verb, *argument;

    // Convert input to lowercase for case-insensitive matching.
    for (int i = 0; input[i]; i++)
    {
        input[i] = (char)tolower((unsigned char)input[i]);
    }

    verb = strtok(input, " \n"); // Tokenize the string by space or newline.
    if (!verb)
        return; // Empty input

    argument = strtok(NULL, " \n");

    // Handle single-word movement commands like "north"
    if (strcmp(verb, "n") == 0 || strcmp(verb, "north") == 0 ||
        strcmp(verb, "s") == 0 || strcmp(verb, "south") == 0 ||
        strcmp(verb, "e") == 0 || strcmp(verb, "east") == 0 ||
        strcmp(verb, "w") == 0 || strcmp(verb, "west") == 0)
    {
        argument = verb; // The argument to "go" is the verb itself.
        verb = "go";
    }

    for (int i = 0; command_table[i].command != NULL; i++)
    {
        if (strcmp(verb, command_table[i].command) == 0)
        {
            // We found a match! Call the associated function pointer.
            command_table[i].handler(game, argument);
            return;
        }
    }

    ui_log(game, "I don't understand that command.");
}

void handle_quit(GameState *game, char *argument)
{
    (void)argument;
    game->game_should_close = 1;
}

void handle_look(GameState *game, char *argument)
{
    (void)argument;
    Room *room = game->player.current_room;
    ui_log(game, room->description);

    // `strcat` does not check sizes, so we made the buffer big enough for the
    // longest possible result: "Exits: north south east west " is 29
    // characters plus the `\0`, well under 100.
    char exits_str[100] = "Exits: ";
    int found_exit = 0;
    for (int i = 0; i < MAX_DIRECTIONS; i++)
    {
        if (room->exits[i])
        {
            // `i` is an `int`; the cast says we mean it as a `Direction`
            // (the enum values are 0 to MAX_DIRECTIONS - 1).
            strcat(exits_str, direction_to_string((Direction)i));
            strcat(exits_str, " ");
            found_exit = 1;
        }
    }
    if (found_exit)
    {
        ui_log(game, exits_str);
    }
    else
    {
        ui_log(game, "There are no obvious exits.");
    }
}

void handle_go(GameState *game, char *argument)
{
    if (!argument)
    {
        ui_log(game, "Go where?");
        return;
    }

    int dir = -1;
    if (strcmp(argument, "n") == 0 || strcmp(argument, "north") == 0)
        dir = NORTH;
    else if (strcmp(argument, "s") == 0 || strcmp(argument, "south") == 0)
        dir = SOUTH;
    else if (strcmp(argument, "e") == 0 || strcmp(argument, "east") == 0)
        dir = EAST;
    else if (strcmp(argument, "w") == 0 || strcmp(argument, "west") == 0)
        dir = WEST;

    if (dir == -1)
    {
        ui_log(game, "That's not a valid direction.");
        return;
    }

    Room *next_room = game->player.current_room->exits[dir];
    if (next_room)
    {
        game->player.current_room = next_room;
        handle_look(game, NULL); // Automatically look after moving.
    }
    else
    {
        ui_log(game, "You can't go that way.");
    }
}

void handle_help(GameState *game, char *argument)
{
    (void)argument;
    ui_log(game, "--- Available Commands ---");
    ui_log(game, "look (l): Describe the current room and exits.");
    ui_log(game, "go <dir>: Move in a direction (north, south, east, west, or n,s,e,w).");
    ui_log(game, "help: Show this help message.");
    ui_log(game, "quit/exit: Leave the game.");
}

// =====================================================================================
// |                                - NCURSES UI CODE -                                |
// =====================================================================================

/**
 * @brief Starts ncurses and creates the game's three windows.
 * @return 1 on success, 0 if the terminal is too small or a window could not
 *         be created (the terminal is restored and an error printed).
 */
int ui_init(GameState *game)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);

    // The layout needs a few rows for the log plus 5 for the status and input
    // windows. On a tinier terminal the window sizes below would be zero or
    // negative, so we stop with a clear message instead.
    if (height < 10 || width < 40)
    {
        endwin();
        fprintf(stderr, "Error: The terminal must be at least 40 columns by 10 rows.\n");
        return 0;
    }

    // Create a main window for game text, a status window, and an input window.
    // Like `malloc`, `newwin` returns NULL when it fails, so we check all three.
    game->main_win = newwin(height - 5, width, 0, 0);
    game->status_win = newwin(3, width, height - 5, 0);
    game->input_win = newwin(2, width, height - 2, 0);
    if (!game->main_win || !game->status_win || !game->input_win)
    {
        ui_cleanup(game); // delwin() safely ignores the NULL ones.
        fprintf(stderr, "Error: Could not create the game windows.\n");
        return 0;
    }

    scrollok(game->main_win, TRUE); // Allow the main window to scroll.

    wborder(game->status_win, 0, 0, 0, 0, 0, 0, 0, 0);
    wborder(game->input_win, ' ', ' ', 0, 0, '>', '<', 0, 0);

    refresh();
    return 1;
}

void ui_draw(GameState *game)
{
    // Draw status window
    box(game->status_win, 0, 0);
    mvwprintw(game->status_win, 1, 2, "Location: Room %d", game->player.current_room->id);

    // Draw input window
    box(game->input_win, 0, 0);
    mvwprintw(game->input_win, 1, 2, "> ");

    // Draw main window log messages. A room description is often wider than
    // the terminal, so it WRAPS onto the next line. Printing message `i` at a
    // fixed row such as `i + 1` would draw the next message over that wrapped
    // text. Instead we start at row 1 and let each message (ending in "\n")
    // leave the cursor where the next one should begin. Because the window
    // has `scrollok` on, text that runs past the bottom scrolls the oldest
    // lines away, so the newest messages are always visible.
    werase(game->main_win);
    wmove(game->main_win, 1, 0);
    for (int i = 0; i < game->log_count; i++)
    {
        wprintw(game->main_win, "  %s\n", game->log_messages[i]);
    }

    // Refresh all windows to show changes
    wrefresh(game->main_win);
    wrefresh(game->status_win);
    wrefresh(game->input_win);
}

void ui_log(GameState *game, const char *message)
{
    char *message_copy = duplicate_string(message);

    if (!message_copy)
    {
        return;
    }

    // If the log is full, shift all messages up.
    if (game->log_count >= MAX_LOG_MESSAGES)
    {
        free(game->log_messages[0]);
        for (int i = 0; i < MAX_LOG_MESSAGES - 1; i++)
        {
            game->log_messages[i] = game->log_messages[i + 1];
        }
        game->log_count--;
    }

    game->log_messages[game->log_count++] = message_copy;
}

/**
 * @brief Reads one command from the player.
 * @return 1 if a line was read, 0 if input has ended or failed.
 */
int ui_get_input(GameState *game, char *buffer)
{
    // Move cursor to input window and get string from user.
    wmove(game->input_win, 1, 4);

    // Like `scanf` and `fgets`, `wgetnstr` can fail, for example when the
    // terminal goes away. It then returns ERR and may not have written
    // anything into `buffer`, so we must not use the buffer's contents.
    if (wgetnstr(game->input_win, buffer, INPUT_BUFFER_SIZE - 1) == ERR)
    {
        buffer[0] = '\0';
        return 0;
    }
    return 1;
}

void ui_cleanup(GameState *game)
{
    delwin(game->main_win);
    delwin(game->status_win);
    delwin(game->input_win);
    endwin(); // Restore terminal to normal state. CRITICAL.
}

// =====================================================================================
// |                                 - GAME LIFECYCLE -                                |
// =====================================================================================

void game_loop(GameState *game)
{
    char input_buffer[INPUT_BUFFER_SIZE] = "";

    while (!game->game_should_close)
    {
        ui_draw(game);
        if (!ui_get_input(game, input_buffer))
        {
            // No more input can arrive. Without this check the loop would
            // spin forever on the same failed read.
            game->game_should_close = 1;
            break;
        }
        parse_and_execute_command(game, input_buffer);
    }
}

/**
 * @brief Frees all dynamically allocated memory.
 * This is CRITICAL for preventing memory leaks. A C developer must always
 * clean up their own mess.
 */
void cleanup(GameState *game)
{
    // Free all the room structs
    for (int i = 0; i < game->num_rooms; i++)
    {
        free(game->all_rooms[i]);
    }
    // Free all the log message strings
    for (int i = 0; i < game->log_count; i++)
    {
        free(game->log_messages[i]);
    }
    // Finally, free the main GameState struct itself.
    free(game);
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * HOW TO COMPILE AND RUN THIS PROJECT:
 *
 * This lesson is still a single-source-file program. You can compile it with
 * one command, then point it at any compatible map file you create. Because it
 * depends on `ncurses`, it is aimed at Unix-like environments unless you install
 * a compatible curses implementation for your platform.
 *
 * 1. CREATE THE DATA FILE:
 *    In the same directory, create a file named `world.map`. This file defines
 *    the rooms and their connections. Copy this content into it:
 *
 *    room 0 "You are in a dusty, forgotten library. Ancient tomes line the walls. A grand wooden door stands to the north."
 *    room 1 "You've entered a vast, cold cavern. The sound of dripping water echoes around you. A narrow passage leads east."
 *    room 2 "This is a small, cramped tunnel. It smells of damp earth. The path continues west, and you see a faint light to the south."
 *    room 3 "You emerge into a treasure room! Piles of gold and jewels glitter in the torchlight. A heavy stone door is to the north."
 *
 *    link 0 n 1
 *    link 1 s 0
 *    link 1 e 2
 *    link 2 w 1
 *    link 2 s 3
 *    link 3 n 2
 *
 * 2. COMPILE THE PROGRAM:
 *    Open a terminal in the directory containing this file and link `ncurses`:
 *
 *      `cc -Wall -Wextra -std=c11 -o 35_capstone_awesome_text_adventure 35_capstone_awesome_text_adventure.c -lncurses`
 *
 * 3. RUN THE GAME:
 *    Execute the program, passing the map file as a command-line argument:
 *
 *      `./35_capstone_awesome_text_adventure world.map`
 *
 *    Your terminal will transform, and the game will begin.
 */
