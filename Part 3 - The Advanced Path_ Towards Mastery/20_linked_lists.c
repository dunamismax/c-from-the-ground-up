/**
 * @file 20_linked_lists.c
 * @brief Part 3, Lesson 20: Linked Lists
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file serves as the lesson and demonstration for a singly linked list.
 * It shows how to build one of the most fundamental dynamic data structures
 * from scratch using structs, pointers, and dynamic memory allocation.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * So far, our primary way of storing a collection of items has been the ARRAY.
 * Arrays are great, but they have one major limitation: they have a fixed size.
 * If you declare `int my_array[100];`, you can't store 101 items without
 * creating a new, larger array and copying everything over.
 *
 * Welcome to the LINKED LIST, a data structure that solves this problem.
 *
 * WHAT IS A LINKED LIST?
 * A LINKED LIST is a sequence of data elements, which are connected together via
 * links. Each element, called a NODE, contains two things:
 * 1. The DATA itself (e.g., a number, a struct, etc.).
 * 2. A POINTER to the next node in the sequence.
 *
 * Think of it like a train. Each `Node` is a train car. It holds some cargo (the
 * data) and has a coupling that connects it to the next car (the `next` pointer).
 * All we need to know to find the entire train is where the first car is. This
 * first-car pointer is called the HEAD.
 */

#include <stdio.h>
#include <stdlib.h> // For malloc() and free()

// --- Part 1: The Building Block - The Node ---

// This is the blueprint for a single "car" in our train.
// It's a SELF-REFERENTIAL struct because it contains a pointer to itself.
typedef struct Node
{
    int data;          // The data this node holds
    struct Node *next; // A pointer to the next node in the list
} Node;

// --- Part 2: Core List Operations ---
// We will write functions to handle the list's main operations.

/**
 * @brief Creates a new node, allocates memory for it, and initializes its fields.
 * @param data The integer data to store in the new node.
 * @return A pointer to the newly created node.
 */
Node *create_node(int data)
{
    // Allocate memory for one Node on the HEAP.
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        exit(1);
    }
    new_node->data = data; // Set the data.
    new_node->next = NULL; // The new node doesn't point to anything yet.
    return new_node;
}

/**
 * @brief Inserts a new node at the beginning of the list.
 * @param head_ref A pointer to the head pointer. We need this double pointer
 *                 so we can modify the `head` pointer in the main function.
 * @param data The data for the new node.
 */
void insert_at_beginning(Node **head_ref, int data)
{
    // 1. Create the new node.
    Node *new_node = create_node(data);

    // 2. Point the new node's `next` to what `head` is currently pointing to.
    new_node->next = *head_ref;

    // 3. Update `head` to point to our new node, making it the new first node.
    *head_ref = new_node;
}

/**
 * @brief Prints all the elements of the list from beginning to end.
 * @param head A pointer to the first node of the list.
 */
void print_list(Node *head)
{
    Node *current = head; // Start at the beginning.

    while (current != NULL)
    {
        printf("%d -> ", current->data);
        current = current->next; // Move to the next node.
    }
    printf("NULL\n"); // The end of the list points to nothing.
}

/**
 * @brief Frees all memory allocated for the list to prevent memory leaks.
 * @param head_ref A pointer to the head pointer.
 */
void free_list(Node **head_ref)
{
    Node *current = *head_ref;
    Node *temp;

    while (current != NULL)
    {
        temp = current;          // Save the current node.
        current = current->next; // Move to the next one.
        free(temp);              // Free the saved node.
    }

    // Finally, set the original head pointer in main() to NULL.
    *head_ref = NULL;
}

int main(void)
{
    // The HEAD pointer. This is our only entry point into the list.
    // An empty list is represented by a NULL head pointer.
    Node *head = NULL;

    printf("Building the linked list by inserting at the beginning...\n");

    // Insert elements. Since we insert at the beginning, the last one
    // we insert will be the first one in the list.
    insert_at_beginning(&head, 30);
    insert_at_beginning(&head, 20);
    insert_at_beginning(&head, 10);

    printf("The current list is:\n");
    print_list(head);
    printf("\n");

    printf("Adding another element, 5...\n");
    insert_at_beginning(&head, 5);

    printf("The final list is:\n");
    print_list(head);
    printf("\n");

    // CRITICAL STEP: Always clean up memory when you're done.
    printf("Freeing all nodes in the list...\n");
    free_list(&head);

    printf("List after freeing:\n");
    print_list(head); // Should print "NULL"

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * Key Takeaways:
 *
 * 1.  A LINKED LIST is a dynamic data structure made of NODES linked by pointers.
 * 2.  A NODE contains DATA and a POINTER to the next node.
 * 3.  The HEAD pointer is the entry point to the entire list. If `head` is `NULL`,
 *     the list is empty.
 * 4.  Nodes are created on the HEAP using `malloc()`, which gives us the flexibility
 *     to grow or shrink the list at runtime.
 * 5.  Because we use `malloc()`, we are responsible for using `free()` on every
 *     single node to prevent memory leaks.
 * 6.  To modify the head pointer from within a function, we must pass its
 *     address (a pointer to a pointer, or `Node **`).
 *
 * You have just built one of the most fundamental data structures in all of
 * computer science. Understanding linked lists is key to tackling more complex
 * structures like trees, graphs, and hash tables.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * 1. Open a terminal or command prompt.
 * 2. Navigate to the directory where you saved this file.
 * 3. Use the GCC compiler to create an executable file:
 *    `gcc -Wall -Wextra -std=c11 -o 20_linked_lists 20_linked_lists.c`
 * 4. Run the executable:
 *    - On Linux/macOS:   `./20_linked_lists`
 *    - On Windows:       `20_linked_lists.exe`
 */