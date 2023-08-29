#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

double calculate(double num1, double num2, char operator) {
    switch (operator) {
        case '+':
            return num1 + num2;
        case '-':
            return num1 - num2;
        case '*':
            return num1 * num2;
        case '/':
            return num1 / num2;
        default:
            return 0.0;
    }
}

int main() {
    int server_fd, client_socket, addr_len;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Calculator Server is running...\n");

    while (1) {
        addr_len = sizeof(client_addr);

        // Accept incoming connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive data from client
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        buffer[bytes_received] = '\0';

        double num1, num2, result;
        char operator;

        sscanf(buffer, "%lf %c %lf", &num1, &operator, &num2);

        // Perform calculation
        result = calculate(num1, num2, operator);

        // Convert result to string
        char result_str[MAX_BUFFER_SIZE];
        snprintf(result_str, sizeof(result_str), "%.2lf", result);

        // Send result back to client
        send(client_socket, result_str, strlen(result_str), 0);

        printf("Result sent: %s\n", result_str);

        close(client_socket);
    }

    return 0;
}
