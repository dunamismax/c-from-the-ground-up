/**
 * @file 35_capstone_awesome_text_adventure.c
 * @brief Part 5, Lesson 35: Capstone - Awesome Text Adventure
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file contains the complete source code for the final capstone project.
 * It integrates nearly every concept taught in this course into a single,
 * functional, and fun application: a text-based adventure game.
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
 * - Parsing: `sscanf` is used to parse the world data file.
 * - Command-Line Arguments: The program takes the map file as an argument.
 * - External Libraries: We use the `ncurses` library for an advanced terminal UI.
 * - Multi-file Builds: This project is designed to be built with a `Makefile`,
 *   the standard for any non-trivial C application.
 * - Advanced Techniques: We'll even use an array of structs with function
 *   pointers for a clean command handling system.
 *
 * You are no longer a beginner. You are a C developer. Let's build something awesome.
 */

// --- Standard and External Library Includes ---
#include <ncurses.h> // For the advanced Terminal User Interface (TUI)
#include <stdio.h>
#include <stdlib.h> // For malloc, free, exit
#include <string.h> // For string manipulation functions
#include <ctype.h>  // For tolower()

// --- Game Constants and Enums ---
#define MAX_DIRECTIONS 4
#define MAX_ROOMS 100
#define MAX_LOG_MESSAGES 10
#define INPUT_BUFFER_SIZE 100

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
    char description[512];
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

// Command Handling
void parse_and_execute_command(GameState *game, char *input);
void handle_quit(GameState *game, char *argument);
void handle_look(GameState *game, char *argument);
void handle_go(GameState *game, char *argument);
void handle_help(GameState *game, char *argument);

// UI Functions (ncurses)
void ui_init(GameState *game);
void ui_draw(GameState *game);
void ui_get_input(GameState *game, char *buffer);
void ui_log(GameState *game, const char *message);
void ui_cleanup(GameState *game);

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

    ui_init(game); // Initialize the ncurses interface

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

    char line[512];

    // --- TWO-PASS LOADING ---
    // This is a common and robust technique for loading interconnected data.
    // Pass 1: Read all room definitions and allocate memory for them.
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "room", 4) == 0)
        {
            if (!parse_room(line, game))
            {
                fprintf(stderr, "Error parsing room definition.\n");
                fclose(file);
                cleanup(game);
                return NULL;
            }
        }
    }

    // Rewind the file pointer to the beginning for the second pass.
    rewind(file);

    // Pass 2: Read all link definitions and connect the already-created rooms.
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "link", 4) == 0)
        {
            if (!parse_link(line, game))
            {
                fprintf(stderr, "Error parsing link definition.\n");
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
    if (game->num_rooms >= MAX_ROOMS)
        return 0; // Guard against overflow.

    int id;
    char desc[512];

    // Using sscanf to parse a specific format from a string.
    if (sscanf(line, "room %d \"%[^\"]\"", &id, desc) != 2)
    {
        return 0;
    }

    Room *new_room = malloc(sizeof(Room));
    if (!new_room)
        return 0;

    memset(new_room, 0, sizeof(Room)); // Initialize room exits to NULL.
    new_room->id = id;
    strcpy(new_room->description, desc);

    game->all_rooms[game->num_rooms++] = new_room;
    return 1;
}

/**
 * @brief Parses a 'link' line and connects two existing rooms.
 */
int parse_link(char *line, GameState *game)
{
    int from_id, to_id;
    char dir_str[10];

    if (sscanf(line, "link %d %s %d", &from_id, dir_str, &to_id) != 3)
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
        input[i] = tolower(input[i]);
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
    game->game_should_close = 1;
}

void handle_look(GameState *game, char *argument)
{
    Room *room = game->player.current_room;
    ui_log(game, room->description);

    char exits_str[100] = "Exits: ";
    int found_exit = 0;
    for (int i = 0; i < MAX_DIRECTIONS; i++)
    {
        if (room->exits[i])
        {
            strcat(exits_str, direction_to_string(i));
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

    Direction dir = -1;
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
    ui_log(game, "--- Available Commands ---");
    ui_log(game, "look (l): Describe the current room and exits.");
    ui_log(game, "go <dir>: Move in a direction (north, south, east, west, or n,s,e,w).");
    ui_log(game, "help: Show this help message.");
    ui_log(game, "quit/exit: Leave the game.");
}

// =====================================================================================
// |                                - NCURSES UI CODE -                                |
// =====================================================================================

void ui_init(GameState *game)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);

    // Create a main window for game text, a status window, and an input window.
    game->main_win = newwin(height - 5, width, 0, 0);
    game->status_win = newwin(3, width, height - 5, 0);
    game->input_win = newwin(2, width, height - 2, 0);

    scrollok(game->main_win, TRUE); // Allow the main window to scroll.

    wborder(game->status_win, 0, 0, 0, 0, 0, 0, 0, 0);
    wborder(game->input_win, ' ', ' ', 0, 0, '>', '<', 0, 0);

    refresh();
}

void ui_draw(GameState *game)
{
    int width;
    width = getmaxx(stdscr);

    // Draw status window
    box(game->status_win, 0, 0);
    mvwprintw(game->status_win, 1, 2, "Location: Room %d", game->player.current_room->id);

    // Draw input window
    box(game->input_win, 0, 0);
    mvwprintw(game->input_win, 1, 2, "> ");

    // Draw main window log messages
    werase(game->main_win);
    for (int i = 0; i < game->log_count; i++)
    {
        mvwprintw(game->main_win, i + 1, 2, "%s", game->log_messages[i]);
    }

    // Refresh all windows to show changes
    wrefresh(game->main_win);
    wrefresh(game->status_win);
    wrefresh(game->input_win);
}

void ui_log(GameState *game, const char *message)
{
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
    // strdup is a convenient (but non-standard) function that combines malloc and strcpy.
    game->log_messages[game->log_count++] = strdup(message);
}

void ui_get_input(GameState *game, char *buffer)
{
    // Move cursor to input window and get string from user.
    wmove(game->input_win, 1, 4);
    wgetnstr(game->input_win, buffer, INPUT_BUFFER_SIZE - 1);
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
    char input_buffer[INPUT_BUFFER_SIZE];

    while (!game->game_should_close)
    {
        ui_draw(game);
        ui_get_input(game, input_buffer);
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
 * This is a true, multi-part C project. You cannot compile it with a simple
 * `gcc` command. You must create the supporting files and use `make`.
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
 * 2. CREATE THE MAKEFILE:
 *    This project requires linking the `ncurses` library. A Makefile is the
 *    standard way to manage this. Create a file named `Makefile` (with a
 *    capital M, no extension) and copy this content into it:
 *
 *    # Makefile for the Awesome Text Adventure
 *    CC = gcc
 *    CFLAGS = -Wall -Wextra -std=c11
 *    LDFLAGS = -lncurses
 *    TARGET = 35_capstone_awesome_text_adventure
 *    SRC = 35_capstone_awesome_text_adventure.c
 *
 *    all: $(TARGET)
 *
 *    $(TARGET): $(SRC)
 *        $(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)
 *
 *    clean:
 *        rm -f $(TARGET)
 *
 * 3. COMPILE THE PROJECT:
 *    Open a terminal in the directory containing all three files (`.c`, `.map`, `Makefile`).
 *    Simply run the `make` command:
 *
 *      `make`
 *
 *    This will automatically execute the correct `gcc` command with all the
 *    necessary flags and libraries.
 *
 * 4. RUN THE GAME:
 *    Execute the program, passing the map file as a command-line argument:
 *
 *      `./35_capstone_awesome_text_adventure world.map`
 *
 *    Your terminal will transform, and the game will begin. Congratulations,
 *    you have completed C From The Ground Up!
 */