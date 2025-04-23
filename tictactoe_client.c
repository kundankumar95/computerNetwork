#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Server info
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 3. Connect
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connected to server.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("%s", buffer);

        if (strncmp(buffer, "Invalid", 7) == 0 || strstr(buffer, "turn") != NULL) {
            continue;
        }

        printf("Enter row and column (0-2): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);

        if (strstr(buffer, "wins") != NULL || strstr(buffer, "draw") != NULL) {
            break;
        }
    }

    close(sock);
    return 0;
}



// gcc tictactoe_server.c -o tictactoe_server
// gcc tictactoe_client.c -o tictactoe_client

// # Terminal 1 (Server)
// ./tictactoe_server

// # Terminal 2 (Client 1)
// ./tictactoe_client

// # Terminal 3 (Client 2)
// ./tictactoe_client
