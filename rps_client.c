#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char client_move[BUFFER_SIZE];

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Server info
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 3. Connect
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connected to server. Type 'exit' to quit.\n");

    while (1) {
        printf("Enter your move (rock/paper/scissors): ");
        fgets(client_move, BUFFER_SIZE, stdin);
        client_move[strcspn(client_move, "\n")] = 0;

        send(sock, client_move, strlen(client_move), 0);

        if (strncmp(client_move, "exit", 4) == 0) break;

        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("Result: %s\n", buffer);
    }

    close(sock);
    return 0;
}
