#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];
    socklen_t addrlen = sizeof(address);

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 3. Listen
    listen(server_fd, 3);
    printf("Server listening on port %d...\n", PORT);

    // 4. Accept
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(new_socket, buffer, BUFFER_SIZE, 0);
        printf("Client: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0) break;

        printf("Server: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(new_socket, message, strlen(message), 0);

        if (strncmp(message, "exit", 4) == 0) break;
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
