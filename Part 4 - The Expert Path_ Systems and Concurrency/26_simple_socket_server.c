/**
 * @file 26_simple_socket_server.c
 * @brief Part 4, Lesson 26: Simple Socket Server
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file implements the server side of our basic TCP client-server application.
 * This program will wait for a client to connect, receive a message,
 * send a reply, and then shut down.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * This is the SERVER component of our application. A server's primary role is
 * to listen for and accept connections from clients.
 *
 * While the client actively initiates contact, the server is passive. It sets
 * up a 'shop' at a known address (IP and PORT) and waits for customers (clients)
 * to arrive.
 *
 * THE SERVER'S JOURNEY
 * A typical TCP server program follows these steps, which are different from
 * the client's:
 *
 * 1. Create a SOCKET. (Same as the client)
 * 2. BIND the socket to a specific IP address and port number. This is how clients
 *    will find it.
 * 3. LISTEN on that port for incoming connection requests. This puts the socket
 *    into a "listening state."
 * 4. ACCEPT a connection from a client. This creates a NEW socket dedicated to
 *    communicating with that specific client. The original socket remains
 *    listening for other clients.
 * 5. RECEIVE and SEND data with the connected client using the new socket.
 * 6. CLOSE the client's connection and, eventually, the main listening socket.
 *
 * Key server-specific functions are `bind()`, `listen()`, and `accept()`.
 */

// --- Required Headers ---
#include <stdio.h>      // For standard I/O
#include <stdlib.h>     // For exit() and atoi()
#include <string.h>     // For string manipulation
#include <unistd.h>     // For close(), write()
#include <sys/socket.h> // The main header for socket programming
#include <arpa/inet.h>  // For address structures and functions

int main(int argc, char *argv[])
{
    // --- Step 0: Validate Command-Line Arguments ---
    // The server needs to know which port to listen on.
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[2]);

    // --- Part 1: Create the Server Socket ---

    int server_socket, client_socket;

    // Create the socket. Same as the client.
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Could not create server socket");
        return 1;
    }
    printf("Server socket created.\n");

    // --- Part 2: Bind the Socket to an IP and Port ---

    struct sockaddr_in server_addr, client_addr;

    // Prepare the sockaddr_in structure for the server.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); // The port to listen on.

    /*
     * INADDR_ANY is a special constant that tells the socket to bind to all
     * available network interfaces on the machine (e.g., Wi-Fi, Ethernet, etc.).
     * This is the standard way to configure a server so it can accept connections
     * from both the local machine (127.0.0.1) and from other machines on the
     * network.
     */
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /*
     * The `bind()` function assigns the address specified by `server_addr` to
     * the socket descriptor `server_socket`. This is a critical step for a server.
     */
    printf("Binding socket to port %d...\n", port);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }
    printf("Bind successful.\n");

    // --- Part 3: Listen for Connections ---

    /*
     * The `listen()` function puts the server socket into a passive mode, where
     * it waits for the client to approach the server to make a connection.
     * The second argument (3) is the BACKLOG, which is the maximum number of
     * pending connections that can be queued up before the server starts
     * refusing new ones.
     */
    listen(server_socket, 3);
    printf("Server listening on port %d...\n", port);
    printf("Waiting for incoming connections...\n");

    // --- Part 4: Accept Incoming Connections ---

    socklen_t client_addr_size = sizeof(struct sockaddr_in);

    /*
     * The `accept()` function is a BLOCKING call. The program will pause here
     * and wait until a client connects.
     * When a connection is accepted, it creates a NEW SOCKET descriptor
     * (`client_socket`) for this specific communication channel.
     * The original `server_socket` remains open and listening for more connections.
     * It also fills the `client_addr` struct with the client's information.
     */
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);

    if (client_socket < 0)
    {
        perror("Accept failed");
        close(server_socket);
        return 1;
    }

    // `inet_ntoa` converts the client's network address into a human-readable string.
    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // --- Part 5: Communicate with the Client ---

    char client_message[2000];
    int read_size;

    // Clear the buffer before reading into it.
    memset(client_message, 0, sizeof(client_message));

    // Receive a message from the client using the NEW socket descriptor.
    // `recv` is also a BLOCKING call.
    read_size = recv(client_socket, client_message, 2000, 0);
    if (read_size > 0)
    {
        printf("Client message: %s\n", client_message);

        // Prepare and send a reply back to the client.
        const char *reply_message = "Message received. Thank you!";
        // `write()` is another function for sending data, similar to `send()`.
        write(client_socket, reply_message, strlen(reply_message));
        printf("Reply sent to client.\n");
    }
    else if (read_size == 0)
    {
        printf("Client disconnected.\n");
    }
    else
    {
        perror("Receive failed");
    }

    // --- Part 6: Close the Sockets ---

    // This simple server handles one client and then shuts down.
    // A real-world server would loop back to `accept()` to handle more clients.
    // We must close BOTH the client-specific socket and the main server socket.
    close(client_socket);
    close(server_socket);
    printf("Sockets closed. Server shutting down.\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * This is the SERVER. It must be running BEFORE you run the client.
 *
 * 1. Open a terminal and compile the server:
 *    `gcc -Wall -Wextra -std=c11 -o 26_simple_socket_server 26_simple_socket_server.c`
 *
 * 2. Run the server, providing a port number for it to listen on.
 *    A common choice for testing is a high-numbered port like 8888.
 *    `./26_simple_socket_server 8888`
 *
 *    The server will start and print "Waiting for incoming connections...".
 *    It is now paused, waiting for a client.
 *
 * 3. Open a SECOND terminal and run the compiled client program as described
 *    in the client's source file.
 *
 *    `./26_simple_socket_client 127.0.0.1 8888 "This is a test!"`
 *
 *    You will see the output in both terminals as they communicate.
 */