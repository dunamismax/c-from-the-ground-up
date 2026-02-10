/**
 * @file 30_multithreaded_file_analyzer.c
 * @brief Part 4, Lesson 30: Multithreaded File Analyzer
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file is a project that harnesses the power of multiple CPU cores
 * by using threads to analyze a large file in parallel.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * PROJECT: HARNESS MULTI-CORE POWER WITH THREADS
 *
 * So far, all our programs have run in a single sequence of execution. They do
 * one thing at a time. This is called single-threaded execution. But modern
 * CPUs have multiple cores, each capable of running instructions independently.
 * How can we take advantage of this power? The answer is MULTITHREADING.
 *
 * PROCESS vs. THREAD
 * - A PROCESS is a running instance of a program (like our `tiny_shell`). It has
 *   its own dedicated memory space, file descriptors, etc. `fork()` creates a new process.
 * - A THREAD is a "lightweight" path of execution *within* a single process.
 *   Multiple threads within the same process SHARE the same memory space. This
 *   makes communication between them easy (they can just access the same variables),
 *   but it also introduces a huge challenge: managing that shared access safely.
 *
 * THE CHALLENGE: RACE CONDITIONS
 * Imagine two threads trying to increment the same global counter (`global_count++`).
 * This operation is not ATOMIC (it's not one single instruction). It's really:
 * 1. Read the value of `global_count` from memory into a CPU register.
 * 2. Increment the value in the register.
 * 3. Write the new value back to memory.
 *
 * A RACE CONDITION occurs if Thread 1 reads the value (say, 5), but before it
 * can write back 6, Thread 2 also reads the value (still 5). Both threads will
 * compute 6 and write it back. The counter should be 7, but it's only 6! We lost an update.
 *
 * THE SOLUTION: MUTEXES
 * A MUTEX (MUTual EXclusion) is a lock. It's a programming primitive that ensures
 * only one thread can access a "critical section" of code at a time. The workflow is:
 * 1. Thread A acquires the lock. No other thread can acquire it until A is done.
 * 2. Thread A safely modifies the shared data.
 * 3. Thread A releases the lock.
 * 4. Thread B, which was waiting, can now acquire the lock and do its work.
 *
 * OUR PLAN:
 * We will write a program that counts the characters, words, and lines in a large
 * file. To speed it up, we will:
 * 1. Read the entire file into one large memory buffer.
 * 2. Divide the buffer into chunks, one for each thread.
 * 3. Launch multiple threads, each running a worker function to analyze its chunk.
 * 4. Each thread will calculate its *local* counts.
 * 5. After finishing, each thread will lock a global mutex, add its local counts
 *    to a global total, and then unlock the mutex.
 * 6. The main thread will wait for all worker threads to complete and then print the result.
 *
 * We will use the POSIX Threads (pthreads) library, the standard for C.
 */

// --- Required Headers ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> // The main header for POSIX Threads
#include <ctype.h>   // For isspace()

// --- Constants and Global Data ---
#define NUM_THREADS 4

// This struct will hold the final, combined results. This is our SHARED DATA.
typedef struct
{
    long long total_chars;
    long long total_words;
    long long total_lines;
} GlobalCounts;

GlobalCounts g_counts = {0, 0, 0}; // Initialize global counts
pthread_mutex_t g_mutex;           // The global MUTEX to protect g_counts

// This struct holds the information we need to pass to each thread.
typedef struct
{
    char *data_chunk; // Pointer to the start of this thread's data
    long chunk_size;  // How many bytes this thread should process
} ThreadData;

// --- The Worker Function ---
// This is the function that each thread will execute.
void *analyze_chunk(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    // --- Step 1: Perform analysis on local variables ---
    // We do NOT want to lock the mutex for every character we count.
    // That would be extremely slow and defeat the purpose of threading.
    // Instead, each thread calculates its own sub-total.
    long long local_chars = 0;
    long long local_words = 0;
    long long local_lines = 0;
    int in_word = 0; // A flag to track if we are inside a word

    for (long i = 0; i < data->chunk_size; i++)
    {
        char c = data->data_chunk[i];
        local_chars++;

        if (c == '\n')
        {
            local_lines++;
        }

        if (isspace(c))
        {
            in_word = 0;
        }
        else if (in_word == 0)
        {
            in_word = 1;
            local_words++;
        }
    }

    // --- Step 2: Lock the mutex and update the global state ---
    // This is the CRITICAL SECTION. Only one thread can be in here at a time.
    pthread_mutex_lock(&g_mutex);

    g_counts.total_chars += local_chars;
    g_counts.total_words += local_words;
    g_counts.total_lines += local_lines;

    pthread_mutex_unlock(&g_mutex); // Release the lock!

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // --- Read entire file into memory ---
    FILE *file = fopen(argv[1], "rb");
    if (!file)
    {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_buffer = malloc(file_size);
    if (!file_buffer)
    {
        fprintf(stderr, "Could not allocate memory for file\n");
        fclose(file);
        return 1;
    }

    if (fread(file_buffer, 1, file_size, file) != file_size)
    {
        fprintf(stderr, "Error reading file\n");
        free(file_buffer);
        fclose(file);
        return 1;
    }
    fclose(file);
    printf("Successfully read %ld bytes from %s.\n", file_size, argv[1]);

    // --- Initialize Threads and Mutex ---
    pthread_t threads[NUM_THREADS];
    ThreadData thread_args[NUM_THREADS];
    pthread_mutex_init(&g_mutex, NULL); // Initialize the mutex

    long chunk_size = file_size / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_args[i].data_chunk = file_buffer + (i * chunk_size);
        thread_args[i].chunk_size = (i == NUM_THREADS - 1) ? (file_size - (i * chunk_size)) : chunk_size;

        printf("Launching thread %d to process %ld bytes.\n", i, thread_args[i].chunk_size);
        // `pthread_create` starts a new thread executing `analyze_chunk`
        // and passes it a pointer to its `thread_args`.
        pthread_create(&threads[i], NULL, analyze_chunk, &thread_args[i]);
    }

    // --- Wait for all threads to complete ---
    for (int i = 0; i < NUM_THREADS; i++)
    {
        // `pthread_join` blocks the main thread until the specified thread finishes.
        pthread_join(threads[i], NULL);
        printf("Thread %d finished.\n", i);
    }

    // --- Clean up and Print Results ---
    pthread_mutex_destroy(&g_mutex); // Always destroy the mutex
    free(file_buffer);

    printf("\n--- Analysis Complete ---\n");
    printf("Total Characters: %lld\n", g_counts.total_chars);
    printf("Total Words:      %lld\n", g_counts.total_words);
    printf("Total Lines:      %lld\n", g_counts.total_lines);
    printf("-------------------------\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * You've just written a parallel program! This is a huge step into high-performance
 * computing. You used POSIX threads to split a task, managed shared data with a
 * mutex to prevent race conditions, and successfully reaped the results.
 *
 * This pattern (split data, process in parallel, safely combine results) is a
 * cornerstone of concurrent programming.
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * Compiling threaded programs requires linking the pthread library. You must add
 * the `-pthread` flag to your GCC command.
 *
 * 1. Compile the program:
 *    `gcc -Wall -Wextra -std=c11 -pthread -o 30_multithreaded_file_analyzer 30_multithreaded_file_analyzer.c`
 *
 * 2. To see the benefit, you need a large file. You can create one with this command
 *    (this creates a ~100MB file of random data, which won't have many lines/words
 *    but is good for testing raw character counting performance):
 *    `dd if=/dev/urandom of=large_test_file.txt bs=1M count=100`
 *    (On Windows, you'll need to find a large text file online to download).
 *
 * 3. Run the analyzer on the large file:
 *    `./30_multithreaded_file_analyzer large_test_file.txt`
 *
 * 4. You can also run it on its own source code for a more readable result:
 *    `./30_multithreaded_file_analyzer 30_multithreaded_file_analyzer.c`
 */