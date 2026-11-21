/**
 * @file test_hash_table.c
 * @brief Unit tests for Lesson 28's hash table.
 *
 * Lesson 38 explains the technique: the lesson file is included with its
 * `main` renamed, so this file can supply its own `main` and call the
 * lesson's functions directly. scripts/smoke_check.sh compiles and runs it.
 */

#include <stdio.h>
#include <string.h>

int lesson_28_main(void); // A prototype for the renamed main.
#define main lesson_28_main
#include "../Part 4 - The Expert Path_ Systems and Concurrency/28_hash_table_implementation.c"
#undef main

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

static int same(const char *found, const char *expected)
{
    return found != NULL && strcmp(found, expected) == 0;
}

int main(void)
{
    // The hash always lands inside the table.
    CHECK(hash_function("") < TABLE_SIZE);
    CHECK(hash_function("a much longer key than the others") < TABLE_SIZE);

    HashTable *table = ht_create();
    CHECK(table != NULL);
    if (table == NULL)
    {
        return 1;
    }

    // Inserted keys can be found; missing keys cannot.
    ht_insert(table, "name", "Alice");
    ht_insert(table, "city", "Paris");
    CHECK(same(ht_search(table, "name"), "Alice"));
    CHECK(same(ht_search(table, "city"), "Paris"));
    CHECK(ht_search(table, "missing") == NULL);

    // Inserting an existing key replaces its value.
    ht_insert(table, "name", "Bob");
    CHECK(same(ht_search(table, "name"), "Bob"));

    // Many keys share TABLE_SIZE buckets, so some must collide; every one is
    // still found (separate chaining).
    char key[16];
    for (int i = 0; i < 50; i++)
    {
        snprintf(key, sizeof(key), "key%d", i);
        ht_insert(table, key, key);
    }
    int found = 0;
    for (int i = 0; i < 50; i++)
    {
        snprintf(key, sizeof(key), "key%d", i);
        found += same(ht_search(table, key), key);
    }
    CHECK(found == 50);

    // Deleting removes only that key, and deleting a missing key is harmless.
    ht_delete(table, "city");
    CHECK(ht_search(table, "city") == NULL);
    CHECK(same(ht_search(table, "name"), "Bob"));
    ht_delete(table, "missing");
    CHECK(same(ht_search(table, "name"), "Bob"));

    ht_free(table);

    if (checks_failed > 0)
    {
        printf("Lesson 28 tests: %d of %d checks FAILED.\n", checks_failed, checks_run);
        return 1;
    }
    printf("Lesson 28 tests: all %d checks passed.\n", checks_run);
    return 0;
}
