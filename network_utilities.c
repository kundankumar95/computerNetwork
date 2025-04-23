#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif

void showMenu() {
    printf("\n===== Network Utilities Menu =====\n");
    printf("1. netstat\n");
    printf("2. IP Configuration (ip a / ipconfig)\n");
    printf("3. nslookup\n");
    printf("4. traceroute / tracert\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

void runCommand(const char* cmd) {
    int result = system(cmd);
    if (result == -1) {
        printf("Error executing command.\n");
    }
}

int main() {
    int choice;
    char input[100];

#ifdef _WIN32
    int isWindows = 1;
#else
    int isWindows = 0;
#endif

    do {
        showMenu();
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice) {
            case 1:
                runCommand("netstat -an");
                break;
            case 2:
                if (isWindows)
                    runCommand("ipconfig");
                else
                    runCommand("ip a");  // Use 'ifconfig' if 'ip' is not installed
                break;
            case 3:
                printf("Enter domain (e.g., google.com): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';  // remove newline
                char nslookup_cmd[150];
                snprintf(nslookup_cmd, sizeof(nslookup_cmd), "nslookup %s", input);
                runCommand(nslookup_cmd);
                break;
            case 4:
                printf("Enter target domain/IP: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                char traceroute_cmd[150];
                if (isWindows)
                    snprintf(traceroute_cmd, sizeof(traceroute_cmd), "tracert %s", input);
                else
                    snprintf(traceroute_cmd, sizeof(traceroute_cmd), "traceroute %s", input);
                runCommand(traceroute_cmd);
                break;
            case 5:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 5);

    return 0;
}
