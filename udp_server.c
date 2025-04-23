#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Fill server info
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // 3. Bind
    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
        printf("Client: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0) break;

        printf("Server: ");
        fgets(message, BUFFER_SIZE, stdin);
        sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *) &cliaddr, len);

        if (strncmp(message, "exit", 4) == 0) break;
    }

    close(sockfd);
    return 0;
}
