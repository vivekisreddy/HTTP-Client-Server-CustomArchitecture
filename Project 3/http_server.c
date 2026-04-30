#include <stdio.h>          // Include standard input/output library for printf and fprintf functions
#include <stdlib.h>         // Include standard library for exit function
#include <string.h>         // Include string manipulation functions
#include <sys/socket.h>     // Include socket-related functions and data structures
#include <arpa/inet.h>      // Include definitions for internet operations
#include <unistd.h>         // Include standard symbolic constants and types
#include <signal.h>         // Include definitions for signal handling
#include <sys/types.h>      // Include data types used in system calls
#include <sys/stat.h>       // Include definitions for file status
#include <fcntl.h>          // Include definitions for file control options

#define RCVBUFSIZE 1024    // Define the size of the receive buffer

void DieWithError(char *errorMessage);            // Function prototype for error handling
void HandleClient(int clientSocket);              // Function prototype for handling client requests

int main(int argc, char *argv[]) {
    int servSock;                                   // Server socket descriptor
    int clientSock;                                 // Client socket descriptor
    unsigned short servPort;                        // Server port number
    struct sockaddr_in servAddr;                    // Server address structure
    struct sockaddr_in clientAddr;                  // Client address structure
    unsigned int clientLen;                         // Length of client address structure

    if (argc != 2) {                                // Check if the correct number of command-line arguments are provided
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);  // Print usage instructions to stderr
        exit(1);                                    // Exit with error status
    }

    servPort = atoi(argv[1]);                       // Convert port number string to integer

    // Create socket for incoming connections
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        DieWithError("socket() failed");            // Call DieWithError function if socket creation fails
    }

    // Construct local address structure
    memset(&servAddr, 0, sizeof(servAddr));        // Zero out the server address structure
    servAddr.sin_family = AF_INET;                  // Set address family to IPv4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Allow connections from any IP address
    servAddr.sin_port = htons(servPort);            // Convert port number to network byte order

    // Bind to the local address
    if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        DieWithError("bind() failed");              // Call DieWithError function if bind fails
    }

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, 5) < 0) {
        DieWithError("listen() failed");            // Call DieWithError function if listen fails
    }

    printf("Server started. Listening on port %d...\n", servPort);  // Print server startup message

    // Accept incoming connections
    while (1) {
        clientLen = sizeof(clientAddr);
        // Accept connection from client
        if ((clientSock = accept(servSock, (struct sockaddr *)&clientAddr, &clientLen)) < 0) {
            DieWithError("accept() failed");        // Call DieWithError function if accept fails
        }

        printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));  // Print client IP address

        // Handle client request
        HandleClient(clientSock);
    }

    // Close the server socket
    close(servSock);
    return 0;
}

void DieWithError(char *errorMessage) {
    perror(errorMessage);                           // Print error message to stderr
    exit(1);                                        // Exit with error status
}

void HandleClient(int clientSocket) {
    char httpRequest[RCVBUFSIZE];                   // Buffer to store HTTP request
    ssize_t bytesRcvd;                              // Number of bytes received
    int file;                                       // File descriptor
    char *responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";  // HTTP response header
    char *notFoundResponse = "HTTP/1.1 404 Not Found\r\n\r\n";  // HTTP 404 Not Found response

    // Receive HTTP request from client
    if ((bytesRcvd = recv(clientSocket, httpRequest, RCVBUFSIZE - 1, 0)) <= 0) {
        DieWithError("recv() failed or connection closed prematurely");  // Call DieWithError function if recv fails
    }
    httpRequest[bytesRcvd] = '\0';                  // Null-terminate the received data

    // Extract requested file name from HTTP request
    char *fileName = strtok(httpRequest, " ");      // Tokenize HTTP request using space as delimiter
    if (fileName == NULL || strcmp(fileName, "GET") != 0) {  // Check if GET method is used
        send(clientSocket, notFoundResponse, strlen(notFoundResponse), 0);  // Send 404 Not Found response to client
        close(clientSocket);                        // Close client socket
        return;
    }
    fileName = strtok(NULL, " ");                   // Get next token (requested file name)

    // Open requested file
    if ((file = open(fileName + 1, O_RDONLY)) < 0) { // Open file in read-only mode
        send(clientSocket, notFoundResponse, strlen(notFoundResponse), 0);  // Send 404 Not Found response to client
        close(clientSocket);                        // Close client socket
        return;
    }

    // Send HTTP response header
    send(clientSocket, responseHeader, strlen(responseHeader), 0);  // Send HTTP response header to client

    // Send requested file to client
    char buffer[RCVBUFSIZE];                        // Buffer to read file content
    ssize_t bytesRead;                              // Number of bytes read from file
    while ((bytesRead = read(file, buffer, RCVBUFSIZE)) > 0) {  // Read file content into buffer
        send(clientSocket, buffer, bytesRead, 0);    // Send file content to client
    }

    // Close file and client socket
    close(file);                                    // Close file
    close(clientSocket);                            // Close client socket
}