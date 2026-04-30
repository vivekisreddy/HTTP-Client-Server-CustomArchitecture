#include <stdio.h>                    // Include standard input/output library for printf and fprintf functions
#include <stdlib.h>                   // Include standard library for exit function
#include <string.h>                   // Include string manipulation functions
#include <sys/socket.h>               // Include socket-related functions and data structures
#include <arpa/inet.h>                // Include definitions for internet operations
#include <netdb.h>                    // Include definitions for network database operations
#include <unistd.h>                   // Include standard symbolic constants and types
#include <sys/time.h>                 // Include definitions for time-related functions and structures

#define RCVBUFSIZE 1024              // Define the size of the receive buffer

// Function prototype
void DieWithError(char *errorMessage);

double calculateRTT(struct timeval start, struct timeval end) {  // Function to calculate round-trip time (RTT)
    return (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
}

int main(int argc, char *argv[]) {
    int sock;                                    // Socket descriptor
    struct addrinfo hints, *res, *p;             // Address information structures
    char *serverURL;                             // Server URL
    char *path;                                  // Path to the resource
    unsigned short serverPort;                   // Server port number
    char httpRequest[RCVBUFSIZE];                // HTTP request buffer
    unsigned int httpRequestLen;                 // Length of HTTP request

    int printRTT = 0;                            // Flag to check if -p option is present

    if (argc != 3 && argc != 4) {                // Check if the correct number of command-line arguments are provided
        fprintf(stderr, "Usage: %s [-p] server_url port_number\n", argv[0]);  // Print usage instructions to stderr
        exit(1);                                 // Exit with error status
    }

    // Check for the -p option
    if (argc == 4 && strcmp(argv[1], "-p") == 0) {  // If -p flag is provided
        printRTT = 1;                                // Set printRTT flag to 1
        serverURL = argv[2];                         // Assign server URL from command-line argument
        serverPort = atoi(argv[3]);                  // Convert port number string to integer
    } else {
        serverURL = argv[1];                         // Assign server URL from command-line argument
        serverPort = atoi(argv[2]);                  // Convert port number string to integer
    }

    // Separate path and host
    char *separator = strchr(serverURL, '/');       // Find the first occurrence of '/' in the server URL
    if (separator != NULL) {                         // If '/' is found
        *separator = '\0';                            // Replace it with null terminator
        path = separator + 1;                         // Set path to the substring after the '/'
    } else {
        path = "/";                                   // Set path to root directory
    }

    memset(&hints, 0, sizeof(hints));               // Initialize hints to zero
    hints.ai_family = AF_UNSPEC;                    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;                // Use TCP socket

    // Resolve server address
    int status = getaddrinfo(serverURL, NULL, &hints, &res);  // Get address information for the server
    if (status != 0) {                                        // Check if getaddrinfo was successful
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));  // Print error message to stderr
        exit(1);                                                // Exit with error status
    }

    // Loop through all the results and connect to the first available
    for (p = res; p != NULL; p = p->ai_next) {                  // Iterate through the list of address structures
        // Create socket
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);  // Create a socket using address information
        if (sock == -1) {                                            // Check if socket creation was successful
            perror("socket");                                        // Print error message to stderr
            continue;                                                // Continue to the next address structure
        }

        // Connect to server
        struct sockaddr_in *addr_in = (struct sockaddr_in *)p->ai_addr;  // Cast address structure to sockaddr_in
        addr_in->sin_port = htons(serverPort);                          // Convert port number to network byte order
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {           // Connect to the server
            close(sock);                                                // Close the socket
            perror("connect");                                          // Print error message to stderr
            continue;                                                    // Continue to the next address structure
        }

        break;                                                           // Break out of the loop if connection is successful
    }

    if (p == NULL) {                                                     // If no connection was established
        fprintf(stderr, "Failed to connect\n");                          // Print error message to stderr
        exit(2);                                                          // Exit with error status
    }

    freeaddrinfo(res);                                                    // Free the linked list of address structures

    // Construct HTTP GET request
    snprintf(httpRequest, RCVBUFSIZE, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, serverURL);  // Format HTTP request
    httpRequestLen = strlen(httpRequest);                                   // Calculate length of HTTP request

    // Send HTTP GET request
    if (send(sock, httpRequest, httpRequestLen, 0) != (ssize_t)httpRequestLen) {  // Send HTTP request to the server
        DieWithError("send() sent a different number of bytes than expected");    // Call DieWithError function if send fails
    }

    // Receive HTTP response
    char httpResponse[RCVBUFSIZE];                // Buffer to store HTTP response
    ssize_t bytesRcvd;                             // Number of bytes received
    struct timeval start, end;                      // Structs to hold start and end time
    gettimeofday(&start, NULL);                     // Start timer
    int htmlEndFlag = 0;                            // Flag to indicate if "</html>" is found
    long contentLength = -1;                        // Content length of the HTTP response
    while ((bytesRcvd = recv(sock, httpResponse, RCVBUFSIZE - 1, 0)) > 0) {  // Receive data from the server
        httpResponse[bytesRcvd] = '\0';                                    // Null-terminate the received data
        printf("%s", httpResponse);                                        // Print the received data

        // Check if "</html>" is found in the received data
        if (strstr(httpResponse, "</html>") != NULL) {  // If "</html>" is found
            htmlEndFlag = 1;                              // Set htmlEndFlag to 1
        }

        // Check if "Content-Length:" header is found
        char *contentLengthPtr = strstr(httpResponse, "Content-Length:");
        if (contentLengthPtr != NULL) {                               // If "Content-Length:" header is found
            contentLength = atol(contentLengthPtr + 16);              // Extract content length from the header
        }
    }
    gettimeofday(&end, NULL);                      // End timer

    if (bytesRcvd < 0) {                           // Check if recv failed
        DieWithError("recv() failed");             // Call DieWithError function if recv fails
    }

    // Calculate and print RTT if -p option is present
    if (printRTT) {                                // If -p flag is present
        double rtt = calculateRTT(start, end);     // Calculate RTT
        printf("Round-trip time: %.2f ms\n", rtt); // Print RTT
    }

    if (!htmlEndFlag) {                            // If "</html>" is not found in the received data
        fprintf(stderr, "</html> not found in the received data\n");  // Print error message to stderr
    }

    if (contentLength >= 0) {                      // If content length is found
        printf("Content-Length: %ld\n", contentLength);  // Print content length
    } else {
        printf("Content-Length: unknown\n");             // Print unknown content length
    }

    close(sock);                                   // Close socket
    exit(0);                                       // Exit with success status
}

void DieWithError(char *errorMessage) {
    perror(errorMessage);                         // Print error message to stderr
    exit(1);                                       // Exit with error status
}
