# Simple Socket Server and Client

## The Client

Welcome to the world of network programming! In this lesson and the next, we
will build a complete, working client-server application. This is your gateway
to understanding how programs communicate over the internet.

THE CLIENT-SERVER MODEL
Most network communication follows a CLIENT-SERVER model.
- A SERVER is a program that runs continuously, LISTENS for incoming
  connections on a specific port, and provides a service.
- A CLIENT is a program that actively initiates a connection to a server to
  request that service.

Think of a web browser (client) connecting to a web server (like google.com)
to request a webpage. This file is the CLIENT.

WHAT IS A SOCKET?
A SOCKET is an endpoint for communication between two programs over a network.
You can think of it like a telephone for your program. You create a socket,
'dial' the server's address and port, and then you can send and receive
data through it.

We will be using TCP/IP sockets (specifically, AF_INET and SOCK_STREAM), which
provide a reliable, connection-oriented communication channel. This means data
arrives in order and without errors, like a phone call.

THE CLIENT'S JOURNEY
A typical client program follows these steps:
1. Create a SOCKET.
2. Specify the server's IP address and PORT number.
3. CONNECT to the server.
4. SEND data (a message).
5. RECEIVE a response.
6. CLOSE the connection.

Let's build it!

The `socket()` function creates a communication endpoint and returns a
file descriptor for it.

ARGUMENTS:
1. domain: AF_INET specifies the IPv4 protocol family.
2. type:   SOCK_STREAM specifies a TCP socket (reliable, connection-oriented).
3. protocol: 0 tells the OS to choose the proper protocol (TCP for SOCK_STREAM).

`struct sockaddr_in` is a structure used to store addresses for the AF_INET family.

- sin_family: The address family. Must match the one used in `socket()`.
- sin_port: The port number. We must use `htons()` (Host TO Network Short)
  to convert the port number into NETWORK BYTE ORDER. This ensures that
  computers with different byte ordering can communicate correctly.
- sin_addr.s_addr: The IP address. `inet_addr()` converts the string IP
  (e.g., "127.0.0.1") into the correct binary format in network byte order.

The `connect()` function establishes a connection to the server.
It's a BLOCKING call; it will wait until the connection is made or an
error occurs.

ARGUMENTS:
1. The client's socket descriptor.
2. A pointer to the server's address struct. Note the cast to
   `(struct sockaddr *)`. This is a C convention for generic socket functions.
3. The size of the address structure.

The `send()` function transmits data to the connected socket.
It returns the number of bytes sent, or -1 on error.

The `recv()` function receives data from a socket.
It's a BLOCKING call; the program will pause here until data arrives.
It returns the number of bytes received, 0 if the connection was closed,
or -1 on error.

### Client Source

<!-- source: Part 4 - The Expert Path_ Systems and Concurrency/26_simple_socket_client.c -->
```c
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
 * Welcome to the world of network programming! In this lesson and the next, we
 * will build a complete, working client-server application. This is your gateway
 * to understanding how programs communicate over the internet.
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
#include <stdio.h>      // For standard I/O, like printf() and perror()
#include <stdlib.h>     // For exit() and atoi()
#include <string.h>     // For string manipulation, like strlen() and memset()
#include <unistd.h>     // For close()
#include <sys/socket.h> // The main header for socket programming functions
#include <arpa/inet.h>  // For functions like inet_addr() and htons()

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
    int port = atoi(argv[2]); // `atoi` converts a string to an integer.
    char *message = argv[3];

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
    struct sockaddr_in server_addr;

    /*
     * `struct sockaddr_in` is a structure used to store addresses for the AF_INET family.
     *
     * - sin_family: The address family. Must match the one used in `socket()`.
     * - sin_port: The port number. We must use `htons()` (Host TO Network Short)
     *   to convert the port number into NETWORK BYTE ORDER. This ensures that
     *   computers with different byte ordering can communicate correctly.
     * - sin_addr.s_addr: The IP address. `inet_addr()` converts the string IP
     *   (e.g., "127.0.0.1") into the correct binary format in network byte order.
     */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

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
    int recv_size;

    // It's good practice to clear the buffer before receiving data into it.
    memset(server_reply, 0, sizeof(server_reply));

    /*
     * The `recv()` function receives data from a socket.
     * It's a BLOCKING call; the program will pause here until data arrives.
     * It returns the number of bytes received, 0 if the connection was closed,
     * or -1 on error.
     */
    recv_size = recv(client_socket, server_reply, 2000, 0);
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
 *    `gcc -Wall -Wextra -std=c11 -o 26_simple_socket_client 26_simple_socket_client.c`
 *
 * 2. In a DIFFERENT terminal, compile and run the server (which we will build next):
 *    `gcc -Wall -Wextra -std=c11 -o 26_simple_socket_server 26_simple_socket_server.c`
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
```

## The Server

This is the SERVER component of our application. A server's primary role is
to listen for and accept connections from clients.

While the client actively initiates contact, the server is passive. It sets
up a 'shop' at a known address (IP and PORT) and waits for customers (clients)
to arrive.

THE SERVER'S JOURNEY
A typical TCP server program follows these steps, which are different from
the client's:

1. Create a SOCKET. (Same as the client)
2. BIND the socket to a specific IP address and port number. This is how clients
   will find it.
3. LISTEN on that port for incoming connection requests. This puts the socket
   into a "listening state."
4. ACCEPT a connection from a client. This creates a NEW socket dedicated to
   communicating with that specific client. The original socket remains
   listening for other clients.
5. RECEIVE and SEND data with the connected client using the new socket.
6. CLOSE the client's connection and, eventually, the main listening socket.

Key server-specific functions are `bind()`, `listen()`, and `accept()`.

INADDR_ANY is a special constant that tells the socket to bind to all
available network interfaces on the machine (e.g., Wi-Fi, Ethernet, etc.).
This is the standard way to configure a server so it can accept connections
from both the local machine (127.0.0.1) and from other machines on the
network.

The `bind()` function assigns the address specified by `server_addr` to
the socket descriptor `server_socket`. This is a critical step for a server.

The `listen()` function puts the server socket into a passive mode, where
it waits for the client to approach the server to make a connection.
The second argument (3) is the BACKLOG, which is the maximum number of
pending connections that can be queued up before the server starts
refusing new ones.

The `accept()` function is a BLOCKING call. The program will pause here
and wait until a client connects.
When a connection is accepted, it creates a NEW SOCKET descriptor
(`client_socket`) for this specific communication channel.
The original `server_socket` remains open and listening for more connections.
It also fills the `client_addr` struct with the client's information.

### Server Source

<!-- source: Part 4 - The Expert Path_ Systems and Concurrency/26_simple_socket_server.c -->
```c
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
#include <arpa/inet.h>  // For address structures and functions
#include <errno.h>      // For errno during numeric parsing
#include <stdio.h>      // For standard I/O
#include <stdlib.h>     // For exit() and strtol()
#include <string.h>     // For string manipulation
#include <sys/socket.h> // The main header for socket programming
#include <unistd.h>     // For close(), write()

int main(int argc, char *argv[])
{
    // --- Step 0: Validate Command-Line Arguments ---
    // The server needs to know which port to listen on.
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        return 1;
    }

    char *endptr = NULL;
    errno = 0;
    long parsed_port = strtol(argv[1], &endptr, 10);
    if (errno != 0 || endptr == argv[1] || *endptr != '\0' || parsed_port < 1 || parsed_port > 65535)
    {
        fprintf(stderr, "Error: Port must be a whole number between 1 and 65535.\n");
        return 1;
    }
    int port = (int)parsed_port;

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
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

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
    if (listen(server_socket, 3) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }
    printf("Server listening on port %d...\n", port);
    printf("Waiting for incoming connections...\n");

    // --- Part 4: Accept Incoming Connections ---

    socklen_t client_addr_size = sizeof(client_addr);

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
```

## How to Compile and Run

Build both programs:

```sh
cc -Wall -Wextra -std=c11 -o socket_server 26_simple_socket_server.c
cc -Wall -Wextra -std=c11 -o socket_client 26_simple_socket_client.c
```

Run the server in one terminal:

```sh
./socket_server 8080
```

Connect with the client in another terminal:

```sh
./socket_client 127.0.0.1 8080
```
