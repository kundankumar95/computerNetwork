#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Fill server info
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY; // or inet_addr("127.0.0.1")

    printf("Connected to UDP server.\n");

    while (1) {
        printf("Client: ");
        fgets(message, BUFFER_SIZE, stdin);
        sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));

        if (strncmp(message, "exit", 4) == 0) break;

        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        printf("Server: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0) break;
    }

    close(sockfd);
    return 0;
}
