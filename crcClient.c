#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
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

void send_data_with_crc(int sockfd, unsigned char *data, size_t len) {
    uint32_t crc = crc32(0, data, len);
    send(sockfd, &crc, sizeof(crc), 0);  // Send the CRC first
    send(sockfd, data, len, 0);          // Then send the data
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    unsigned char data[] = "Hello, Server! This is a test message!";
    size_t len = strlen((char *)data);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    // Send data with CRC
    send_data_with_crc(sockfd, data, len);

    close(sockfd);
    return 0;
}
