/**
 * @file test_linked_list.c
 * @brief Unit tests for Lesson 20's linked list.
 *
 * Lesson 38 explains the technique: the lesson file is included with its
 * `main` renamed, so this file can supply its own `main` and call the
 * lesson's functions directly. scripts/smoke_check.sh compiles and runs it.
 */

#include <stdio.h>

int lesson_20_main(void); // A prototype for the renamed main.
#define main lesson_20_main
#include "../Part 3 - The Advanced Path_ Towards Mastery/20_linked_lists.c"
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

int main(void)
{
    // A new node holds its data and points nowhere yet.
    Node *single = create_node(42);
    CHECK(single->data == 42);
    CHECK(single->next == NULL);
    free_list(&single);
    CHECK(single == NULL);

    // insert_at_beginning puts each new value at the front.
    Node *head = NULL;
    insert_at_beginning(&head, 3);
    insert_at_beginning(&head, 2);
    insert_at_beginning(&head, 1);
    CHECK(head != NULL && head->data == 1);
    CHECK(head != NULL && head->next != NULL && head->next->data == 2);
    CHECK(head != NULL && head->next != NULL && head->next->next != NULL &&
          head->next->next->data == 3 && head->next->next->next == NULL);

    // free_list frees every node and empties the list.
    free_list(&head);
    CHECK(head == NULL);

    // Freeing an empty list is safe.
    free_list(&head);
    CHECK(head == NULL);

    if (checks_failed > 0)
    {
        printf("Lesson 20 tests: %d of %d checks FAILED.\n", checks_failed, checks_run);
        return 1;
    }
    printf("Lesson 20 tests: all %d checks passed.\n", checks_run);
    return 0;
}
