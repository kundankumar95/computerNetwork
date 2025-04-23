#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define BUFFER_SIZE 1024

// Function prototypes
void printBoard(char *board, char *buffer);
int checkWin(char *board);
int isDraw(char *board);

int main() {
    int server_fd, client1_fd, client2_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Create a TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Bind to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections (queue up to 2)
    if (listen(server_fd, 2) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Waiting for two players to join...\n");
    
    // Accept first client (Player 1: X)
    if ((client1_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    send(client1_fd, "Welcome Player 1 (X)\n", 23, 0);
    printf("Player 1 connected.\n");
    
    // Accept second client (Player 2: O)
    if ((client2_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    send(client2_fd, "Welcome Player 2 (O)\n", 23, 0);
    printf("Player 2 connected.\n");
    
    // Initialize board positions 1 to 9 as characters '1' ... '9'
    char board[10];
    for (int i = 1; i < 10; i++) {
        board[i] = '0' + i;
    }
    board[0] = '\0';
    
    int currentPlayer = 1; // 1 => Player1 (X), 2 => Player2 (O)
    char buffer[BUFFER_SIZE];
    
    while (1) {
        // Prepare the board display
        memset(buffer, 0, BUFFER_SIZE);
        printBoard(board, buffer);
        // Send the board to both players
        send(client1_fd, buffer, strlen(buffer), 0);
        send(client2_fd, buffer, strlen(buffer), 0);
        
        // Check for a win or a draw
        if (checkWin(board)) {
            char winMsg[BUFFER_SIZE];
            // Note: The win is for the player who made the last valid move.
            // Since we switch the turn after a valid move, currentPlayer now points to the next player.
            // To identify the winning player, invert the current player.
            int winningPlayer = (currentPlayer == 1) ? 2 : 1;
            if (winningPlayer == 1) {
                snprintf(winMsg, BUFFER_SIZE, "Player 1 (X) wins!\n");
            } else {
                snprintf(winMsg, BUFFER_SIZE, "Player 2 (O) wins!\n");
            }
            send(client1_fd, winMsg, strlen(winMsg), 0);
            send(client2_fd, winMsg, strlen(winMsg), 0);
            break;
        }
        
        if (isDraw(board)) {
            char drawMsg[] = "The game is a draw!\n";
            send(client1_fd, drawMsg, strlen(drawMsg), 0);
            send(client2_fd, drawMsg, strlen(drawMsg), 0);
            break;
        }
        
        // Identify the current client's file descriptor and the other one
        int current_fd = (currentPlayer == 1) ? client1_fd : client2_fd;
        int other_fd = (currentPlayer == 1) ? client2_fd : client1_fd;
        
        // Send a prompt to the current player and a waiting message to the other
        char prompt[] = "Your move (enter position number): ";
        send(current_fd, prompt, strlen(prompt), 0);
        char waitMsg[] = "Waiting for opponent's move...\n";
        send(other_fd, waitMsg, strlen(waitMsg), 0);
        
        // Receive move from current player
        char move[BUFFER_SIZE];
        memset(move, 0, BUFFER_SIZE);
        int valread = recv(current_fd, move, BUFFER_SIZE, 0);
        if (valread <= 0) {
            printf("Player disconnected. Ending game.\n");
            break;
        }
        int pos = atoi(move);
        
        // Validate the move – it must be within 1 and 9 and not already taken
        if (pos < 1 || pos > 9 || board[pos] == 'X' || board[pos] == 'O') {
            char invalid[] = "Invalid move, try again.\n";
            send(current_fd, invalid, strlen(invalid), 0);
            continue; // Let the same player retry
        }
        
        // Update the board with the player's marker
        board[pos] = (currentPlayer == 1) ? 'X' : 'O';
        // Switch turn
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }
    
    close(client1_fd);
    close(client2_fd);
    close(server_fd);
    return 0;
}

// Generates a text representation of the board and stores it in 'buffer'
void printBoard(char *board, char *buffer) {
    sprintf(buffer, "\n");
    for (int i = 1; i <= 9; i++) {
        char mark = board[i];
        strcat(buffer, " ");
        strncat(buffer, &mark, 1);
        strcat(buffer, " ");
        if (i % 3 == 0) {
            strcat(buffer, "\n");
            if (i != 9) strcat(buffer, "---+---+---\n");
        } else {
            strcat(buffer, "|");
        }
    }
    strcat(buffer, "\n");
}

// Checks if a winning combination exists
int checkWin(char *board) {
    int wins[8][3] = {
        {1,2,3}, {4,5,6}, {7,8,9},
        {1,4,7}, {2,5,8}, {3,6,9},
        {1,5,9}, {3,5,7}
    };
    for (int i = 0; i < 8; i++) {
        int a = wins[i][0], b = wins[i][1], c = wins[i][2];
        if (board[a] == board[b] && board[b] == board[c])
            return 1;
    }
    return 0;
}

// Determines if the board is full and no move is possible
int isDraw(char *board) {
    for (int i = 1; i <= 9; i++) {
        if (board[i] != 'X' && board[i] != 'O')
            return 0;
    }
    return 1;
}


// gcc tic_tac_toe_server.c -o ttt_server
// gcc tic_tac_toe_client.c -o ttt_client


// ./ttt_server

// ./ttt_client
