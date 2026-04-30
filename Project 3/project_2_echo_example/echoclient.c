#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h> // for gettimeofday()
#define RCVBUFSIZE 1024

void DieWithError(char *errorMessage);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serverAddr;
    unsigned short serverPort;
    char *serverURL;
    char *option = "";
    char httpRequest[RCVBUFSIZE];
    char httpResponse[RCVBUFSIZE];
    unsigned int httpRequestLen;
    struct timeval start, end;
    double rtt_ms;

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [-p] server_url port_number\n", argv[0]);
        exit(1);
    }

    if (argc == 4 && strcmp(argv[1], "-p") == 0) {
        option = "-p";
        serverURL = argv[2];
        serverPort = atoi(argv[3]);
    } else {
        serverURL = argv[1];
        serverPort = atoi(argv[2]);
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverURL);
    serverAddr.sin_port = htons(serverPort);

    // Connect to server
    if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
        DieWithError("connect() failed");

    // Construct HTTP GET request
    sprintf(httpRequest, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", serverURL);
    httpRequestLen = strlen(httpRequest);

    // Send HTTP GET request
    gettimeofday(&start, NULL); // Start measuring RTT
    if (send(sock, httpRequest, httpRequestLen, 0) != httpRequestLen)
        DieWithError("send() sent a different number of bytes than expected");

    if (strcmp(option, "-p") == 0) {
        printf("Measuring RTT...\n");
        gettimeofday(&end, NULL); // Stop measuring RTT
        rtt_ms = (double)(end.tv_usec - start.tv_usec) / 1000.0 + (double)(end.tv_sec - start.tv_sec) * 1000.0;
        printf("RTT: %.2f ms\n", rtt_ms);
    }

    // Receive HTTP response
    int bytesRcvd;
    while ((bytesRcvd = recv(sock, httpResponse, RCVBUFSIZE - 1, 0)) > 0) {
        httpResponse[bytesRcvd] = '\0';
        printf("%s", httpResponse);
    }

    if (bytesRcvd < 0)
        DieWithError("recv() failed");

    close(sock);
    exit(0);
}

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}
