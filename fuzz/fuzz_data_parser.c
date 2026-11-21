/*
 * libFuzzer target for lesson 34's parser, parse_with_sscanf().
 *
 * A FUZZER calls one function over and over with inputs it invents, keeping
 * the ones that reach new code. Built with AddressSanitizer and UBSan, any
 * input that makes the parser read or write out of bounds stops the run and
 * is saved as a "crash-..." file. Run it with `make fuzz`; see
 * scripts/fuzz_check.sh.
 *
 * The lesson is included as it is, with its `main` renamed, the trick lesson
 * 38 teaches for unit tests.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define main data_parser_lesson_main
int data_parser_lesson_main(int argc, char *argv[]);
#include "../Part 5 - Expert Systems & Application Development/34_parsing_data_files.c"
#undef main

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FILE *file;

    // The parser reads from a FILE, so hand it the bytes as a temporary file.
    file = tmpfile();
    if (file == NULL)
    {
        return 0;
    }

    if (fwrite(data, 1, size, file) != size || fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return 0;
    }

    parse_with_sscanf(file);
    fclose(file);
    return 0;
}
