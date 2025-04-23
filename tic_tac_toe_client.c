#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    
    // Create a TCP socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 address from text to binary form (using localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        return -1;
    }
    
    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }
    
    // Main loop: receive messages and send moves when prompted
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (valread <= 0) {
            printf("Server disconnected or error occurred.\n");
            break;
        }
        printf("%s", buffer);
        
        // If the server prompts for "Your move", let the user input a move
        if (strstr(buffer, "Your move") != NULL) {
            char move[BUFFER_SIZE];
            fgets(move, BUFFER_SIZE, stdin);
            send(sock, move, strlen(move), 0);
        }
    }
    
    close(sock);
    return 0;
}
