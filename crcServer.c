#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// CRC-32 polynomial
#define CRC32_POLY 0xEDB88320

// CRC calculation function
uint32_t crc32(uint32_t crc, unsigned char *buf, size_t len) {
    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (int i = 8; i; i--) {
            if (crc & 1)
                crc = (crc >> 1) ^ CRC32_POLY;
            else
                crc >>= 1;
        }
    }
    return ~crc;
}

int receive_data_with_crc(int sockfd) {
    uint32_t received_crc;
    unsigned char buffer[1024];
    
    // Receive the CRC
    ssize_t len = recv(sockfd, &received_crc, sizeof(received_crc), 0);
    if (len <= 0) {
        perror("Failed to receive CRC");
        return -1;
    }

    // Receive the data
    len = recv(sockfd, buffer, sizeof(buffer), 0);
    if (len <= 0) {
        perror("Failed to receive data");
        return -1;
    }

    // Check the CRC
    uint32_t crc = crc32(0, buffer, len);
    if (crc == received_crc) {
        printf("Data received correctly: %s\n", buffer);
        return 1;
    } else {
        printf("Data corruption detected. CRC mismatch.\n");
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

    // Receive and validate data with CRC
    receive_data_with_crc(newsockfd);

    close(newsockfd);
    close(sockfd);
    return 0;
}
