/**
 * @file 26_simple_socket_client.c
 * @brief Part 4, Lesson 26: Simple Socket Client
 * @author dunamismax
 * @date 06-15-2025
 *
 * This file implements the client side of our basic TCP client-server application.
 * This program will connect to a running server, send it a message, and
 * print the server's response to the console.
 */

/*
 * =====================================================================================
 * |                                   - LESSON START -                                  |
 * =====================================================================================
 *
 * Welcome to the world of network programming! In this lesson, we will build a
 * complete, working client-server application in two programs: this CLIENT and
 * a SERVER (`26_simple_socket_server.c`). This is your gateway to understanding
 * how programs communicate over the internet.
 *
 * THE CLIENT-SERVER MODEL
 * Most network communication follows a CLIENT-SERVER model.
 * - A SERVER is a program that runs continuously, LISTENS for incoming
 *   connections on a specific port, and provides a service.
 * - A CLIENT is a program that actively initiates a connection to a server to
 *   request that service.
 *
 * Think of a web browser (client) connecting to a web server (like google.com)
 * to request a webpage. This file is the CLIENT.
 *
 * WHAT IS A SOCKET?
 * A SOCKET is an endpoint for communication between two programs over a network.
 * You can think of it like a telephone for your program. You create a socket,
 * 'dial' the server's address and port, and then you can send and receive
 * data through it.
 *
 * We will be using TCP/IP sockets (specifically, AF_INET and SOCK_STREAM), which
 * provide a reliable, connection-oriented communication channel. This means data
 * arrives in order and without errors, like a phone call.
 *
 * THE CLIENT'S JOURNEY
 * A typical client program follows these steps:
 * 1. Create a SOCKET.
 * 2. Specify the server's IP address and PORT number.
 * 3. CONNECT to the server.
 * 4. SEND data (a message).
 * 5. RECEIVE a response.
 * 6. CLOSE the connection.
 *
 * Let's build it!
 */

// --- Required Headers ---
// We need several headers for network programming.
#include <errno.h>      // For errno, which strtol() sets on overflow
#include <stdio.h>      // For standard I/O, like printf() and perror()
#include <stdlib.h>     // For exit() and strtol()
#include <string.h>     // For string manipulation, like strlen() and memset()
#include <unistd.h>     // For close()
#include <sys/socket.h> // The main header for socket programming functions
#include <arpa/inet.h>  // For functions like inet_pton() and htons()

// This is the function signature we use when we want to accept command-line arguments.
int main(int argc, char *argv[])
{
    // --- Step 0: Validate Command-Line Arguments ---
    // Our client needs to know where the server is and what message to send.
    // We expect: ./program_name <SERVER_IP> <PORT> <MESSAGE>
    if (argc != 4)
    {
        // `fprintf` is like `printf`, but it lets us specify the output stream.
        // `stderr` is the "standard error" stream, the conventional place for errors.
        fprintf(stderr, "Usage: %s <Server IP> <Port> <Message>\n", argv[0]);
        return 1; // Exit with a non-zero status to indicate an error.
    }

    // Parse the arguments from the command line.
    char *server_ip = argv[1];
    char *message = argv[3];

    // The port arrives as a string. We convert it with `strtol` and check it,
    // exactly as the server does: `atoi` would turn a typo such as "88a8" into
    // 88 and silently connect to the wrong port.
    char *endptr = NULL;
    errno = 0;
    long parsed_port = strtol(argv[2], &endptr, 10);
    if (errno != 0 || endptr == argv[2] || *endptr != '\0' || parsed_port < 1 || parsed_port > 65535)
    {
        fprintf(stderr, "Error: Port must be a whole number between 1 and 65535.\n");
        return 1;
    }
    int port = (int)parsed_port;

    // --- Part 1: Create a Socket ---

    // A SOCKET DESCRIPTOR is an integer that uniquely identifies a socket, much like
    // a FILE DESCRIPTOR identifies an open file.
    int client_socket;

    /*
     * The `socket()` function creates a communication endpoint and returns a
     * file descriptor for it.
     *
     * ARGUMENTS:
     * 1. domain: AF_INET specifies the IPv4 protocol family.
     * 2. type:   SOCK_STREAM specifies a TCP socket (reliable, connection-oriented).
     * 3. protocol: 0 tells the OS to choose the proper protocol (TCP for SOCK_STREAM).
     */
    printf("Creating client socket...\n");
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // `socket()` returns -1 on failure. `perror` prints a descriptive system error.
    if (client_socket == -1)
    {
        perror("Could not create socket");
        return 1;
    }
    printf("Socket created.\n");

    // --- Part 2: Configure Server Address ---

    // We need a structure to hold the server's address information.
    // `memset` fills it with zeros first. The struct has fields we never set
    // ourselves (padding, and on some systems a length field), and they must
    // not hold leftover garbage.
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    /*
     * `struct sockaddr_in` is a structure used to store addresses for the AF_INET family.
     *
     * - sin_family: The address family. Must match the one used in `socket()`.
     * - sin_port: The port number. We must use `htons()` (Host TO Network Short)
     *   to convert the port number into NETWORK BYTE ORDER. This ensures that
     *   computers with different byte ordering can communicate correctly.
     * - sin_addr: The IP address. `inet_pton()` ("presentation to network")
     *   converts the string IP (e.g., "127.0.0.1") into the binary format the
     *   network uses. It returns 1 on success and 0 if the string is not a
     *   valid IPv4 address, so we can report a typo instead of connecting to
     *   the wrong place. (Older code uses `inet_addr()`, which cannot tell an
     *   error apart from the real address 255.255.255.255.)
     */
    server_addr.sin_family = AF_INET;
    // `htons` takes a 16-bit unsigned value (`uint16_t`). We checked above
    // that the port is 1-65535, so the cast keeps every possible port intact.
    server_addr.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1)
    {
        fprintf(stderr, "Error: '%s' is not a valid IPv4 address.\n", server_ip);
        close(client_socket);
        return 1;
    }

    // --- Part 3: Connect to the Server ---

    /*
     * The `connect()` function establishes a connection to the server.
     * It's a BLOCKING call; it will wait until the connection is made or an
     * error occurs.
     *
     * ARGUMENTS:
     * 1. The client's socket descriptor.
     * 2. A pointer to the server's address struct. Note the cast to
     *    `(struct sockaddr *)`. This is a C convention for generic socket functions.
     * 3. The size of the address structure.
     */
    printf("Connecting to server at %s:%d...\n", server_ip, port);
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }
    printf("Connected to server.\n");

    // --- Part 4: Send and Receive Data ---

    // Now that we're connected, we can send our message.
    /*
     * The `send()` function transmits data to the connected socket.
     * It returns the number of bytes sent, or -1 on error.
     */
    printf("Sending message: \"%s\"\n", message);
    if (send(client_socket, message, strlen(message), 0) < 0)
    {
        perror("Send failed");
        close(client_socket);
        return 1;
    }

    // Now we wait for the server's reply.
    char server_reply[2000];
    ssize_t recv_size; // `ssize_t` is a signed size type, so it can hold -1.

    // It's good practice to clear the buffer before receiving data into it.
    memset(server_reply, 0, sizeof(server_reply));

    /*
     * The `recv()` function receives data from a socket.
     * It's a BLOCKING call; the program will pause here until data arrives.
     * It returns the number of bytes received, 0 if the connection was closed,
     * or -1 on error.
     *
     * The data we receive is just bytes, with no `\0` at the end. We ask for at
     * most `sizeof(server_reply) - 1` bytes, so the last byte of the buffer is
     * always one of the zeros from `memset`. That guarantees the reply is a
     * proper string when we print it with `%s`, even if the server sends
     * 2000 bytes or more.
     */
    recv_size = recv(client_socket, server_reply, sizeof(server_reply) - 1, 0);
    if (recv_size < 0)
    {
        perror("Receive failed");
        close(client_socket);
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    // --- Part 5: Close the Socket ---

    // Always clean up! Closing the socket releases the resources.
    close(client_socket);
    printf("Connection closed.\n");

    return 0;
}

/*
 * =====================================================================================
 * |                                    - LESSON END -                                   |
 * =====================================================================================
 *
 * HOW TO COMPILE AND RUN THIS CODE:
 *
 * This is the CLIENT part of a two-part application. You must compile and run
 * the server first.
 *
 * 1. Open a terminal and compile the client:
 *    `gcc -Wall -Wextra -std=c17 -o 26_simple_socket_client 26_simple_socket_client.c`
 *
 * 2. In a DIFFERENT terminal, compile and run the server (`26_simple_socket_server.c`):
 *    `gcc -Wall -Wextra -std=c17 -o 26_simple_socket_server 26_simple_socket_server.c`
 *    `./26_simple_socket_server 8888`
 *
 * 3. Go back to the FIRST terminal (for the client) and run it, providing the
 *    server's IP, port, and a message. If the server is on the same machine,
 *    the IP is 127.0.0.1 (localhost).
 *
 *    `./26_simple_socket_client 127.0.0.1 8888 "Hello from the C client!"`
 *
 *    You should see the client connect, send the message, and then print the
 *    server's reply. The server's terminal will show the message it received.
 */
