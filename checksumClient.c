#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
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

void send_data_with_checksum(int sockfd, unsigned char *data, size_t len) {
    uint16_t chk = checksum(data, len);
    send(sockfd, &chk, sizeof(chk), 0);  // Send the checksum first
    send(sockfd, data, len, 0);          // Then send the data
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    unsigned char data[] = "Hello, Server! This is a checksum test!";
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

    // Send data with checksum
    send_data_with_checksum(sockfd, data, len);

    close(sockfd);
    return 0;
}
