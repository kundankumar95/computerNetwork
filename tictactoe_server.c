#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define BUFFER_SIZE 1024

char board[3][3];  // Tic-Tac-Toe board
int turn = 0;      // 0 for Player 1, 1 for Player 2

// Function to initialize the board
void initialize_board() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

// Function to print the board
void print_board() {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%c ", board[i][j]);
            if (j < 2) printf("| ");
        }
        printf("\n");
        if (i < 2) printf("---------\n");
    }
    printf("\n");
}

// Function to check if the current player wins
int check_winner(char player) {
    // Check rows, columns, and diagonals
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return 1;
        }
    }
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return 1;
    }
    return 0;
}

// Function to check for a draw
int check_draw() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return 0;  // Still moves left
            }
        }
    }
    return 1;  // No moves left, it's a draw
}

int main() {
    int server_fd, client_socket[2];
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    int current_player = 0; // Start with player 1

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 3. Listen
    listen(server_fd, 2);
    printf("Server is ready. Waiting for players...\n");

    // 4. Accept two clients
    for (int i = 0; i < 2; i++) {
        client_socket[i] = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        printf("Player %d connected.\n", i + 1);
    }

    // Initialize game
    initialize_board();

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        print_board();

        // Inform the current player to play
        snprintf(buffer, BUFFER_SIZE, "Player %d's turn. Enter row and column (0-2) to place your mark (x/o): ", current_player + 1);
        send(client_socket[current_player], buffer, strlen(buffer), 0);

        // Receive move from the current player
        recv(client_socket[current_player], buffer, BUFFER_SIZE, 0);
        int row, col;
        sscanf(buffer, "%d %d", &row, &col);

        // Check if the move is valid
        if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') {
            board[row][col] = (current_player == 0) ? 'X' : 'O';
        } else {
            send(client_socket[current_player], "Invalid move, try again.\n", 24, 0);
            continue;
        }

        // Check if the current player wins
        if (check_winner((current_player == 0) ? 'X' : 'O')) {
            print_board();
            snprintf(buffer, BUFFER_SIZE, "Player %d wins!\n", current_player + 1);
            send(client_socket[current_player], buffer, strlen(buffer), 0);
            send(client_socket[1 - current_player], buffer, strlen(buffer), 0);
            break;
        }

        // Check for draw
        if (check_draw()) {
            print_board();
            snprintf(buffer, BUFFER_SIZE, "It's a draw!\n");
            send(client_socket[0], buffer, strlen(buffer), 0);
            send(client_socket[1], buffer, strlen(buffer), 0);
            break;
        }

        // Switch turn to the other player
        current_player = 1 - current_player;
    }

    // Close the sockets
    close(client_socket[0]);
    close(client_socket[1]);
    close(server_fd);
    return 0;
}
