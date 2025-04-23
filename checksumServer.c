#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Simple checksum calculation function
uint16_t checksum(unsigned char *data, size_t len) {
    uint16_t sum = 0;
    while (len--) {
        sum += *data++;
        if (sum & 0x10000) {
            sum &= 0xFFFF;
            sum++;
        }
    }
    return ~sum;
}

int receive_data_with_checksum(int sockfd) {
    uint16_t received_chk;
    unsigned char buffer[1024];
    
    // Receive the checksum
    ssize_t len = recv(sockfd, &received_chk, sizeof(received_chk), 0);
    if (len <= 0) {
        perror("Failed to receive checksum");
        return -1;
    }

    // Receive the data
    len = recv(sockfd, buffer, sizeof(buffer), 0);
    if (len <= 0) {
        perror("Failed to receive data");
        return -1;
    }

    // Check the checksum
    uint16_t chk = checksum(buffer, len);
    if (chk == received_chk) {
        printf("Data received correctly: %s\n", buffer);
        return 1;
    } else {
        printf("Data corruption detected. Checksum mismatch.\n");
        return 0;
    }
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Server is listening on port 8080...\n");

    // Accept client connection
    if ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept failed");
        return -1;
    }

    // Receive and validate data with checksum
    receive_data_with_checksum(newsockfd);

    close(newsockfd);
    close(sockfd);
    return 0;
}
