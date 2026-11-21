/*
 * libFuzzer target for lesson 35's map loader: load_world(), parse_room(),
 * parse_link() and cleanup().
 *
 * Every input is written to a file and loaded as a world map. The loader
 * must either build a world or refuse the file; it must never crash, read
 * out of bounds or leak, whatever the bytes are. Run it with `make fuzz`;
 * see scripts/fuzz_check.sh.
 *
 * The lesson is included as it is, with its `main` renamed, the trick lesson
 * 38 teaches for unit tests. Nothing here starts ncurses.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define main capstone_lesson_main
int capstone_lesson_main(int argc, char *argv[]);
#include "../Part 5 - Expert Systems & Application Development/35_capstone_awesome_text_adventure.c"
#undef main

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // FUZZ_MAP_FILE is set by scripts/fuzz_check.sh to a path in its own
    // temporary directory.
    const char *path = getenv("FUZZ_MAP_FILE");
    FILE *file;
    GameState *game;

    if (path == NULL)
    {
        fprintf(stderr, "Set FUZZ_MAP_FILE to a scratch file path.\n");
        abort();
    }

    file = fopen(path, "wb");
    if (file == NULL)
    {
        return 0;
    }

    if (fwrite(data, 1, size, file) != size)
    {
        fclose(file);
        return 0;
    }
    fclose(file);

    game = load_world(path);
    if (game != NULL)
    {
        // What main() does next: look up the starting room. Then walk every
        // exit, which must point at a room the loader created.
        (void)find_room_by_id(game, 0);
        for (int i = 0; i < game->num_rooms; i++)
        {
            for (int d = 0; d < MAX_DIRECTIONS; d++)
            {
                Room *exit_room = game->all_rooms[i]->exits[d];

                if (exit_room != NULL && find_room_by_id(game, exit_room->id) != exit_room)
                {
                    abort();
                }
            }
        }
        cleanup(game);
    }

    return 0;
}
