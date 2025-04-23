#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

const char* determine_winner(const char* server_move, const char* client_move) {
    if (strcmp(server_move, client_move) == 0) return "Draw!";
    if ((strcmp(server_move, "rock") == 0 && strcmp(client_move, "scissors") == 0) ||
        (strcmp(server_move, "paper") == 0 && strcmp(client_move, "rock") == 0) ||
        (strcmp(server_move, "scissors") == 0 && strcmp(client_move, "paper") == 0))
        return "Server Wins!";
    else
        return "Client Wins!";
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char client_move[BUFFER_SIZE];
    char server_move[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 3. Listen
    listen(server_fd, 1);
    printf("Server is ready. Waiting for client...\n");

    // 4. Accept client
    client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    printf("Client connected.\n");

    while (1) {
        memset(client_move, 0, BUFFER_SIZE);
        recv(client_socket, client_move, BUFFER_SIZE, 0);
        if (strncmp(client_move, "exit", 4) == 0) break;

        printf("Client chose: %s\n", client_move);
        printf("Server (you), enter your move (rock/paper/scissors): ");
        fgets(server_move, BUFFER_SIZE, stdin);
        server_move[strcspn(server_move, "\n")] = 0;

        const char* winner = determine_winner(server_move, client_move);
        snprintf(result, BUFFER_SIZE, "Server: %s | Client: %s => %s", server_move, client_move, winner);
        send(client_socket, result, strlen(result), 0);
    }

    close(client_socket);
    close(server_fd);
    return 0;
}
