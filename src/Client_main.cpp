#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <string>
#include <iostream>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Send data to the server
    while (1)
    {
        std::string message;
        std::getline(std::cin, message);
        if (send(sockfd, message.c_str(), strlen(message.c_str()), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        // Receive response from the server
        char buffer[1024];
        ssize_t bytes_received;
        if ((bytes_received = recv(sockfd, buffer, sizeof(buffer), 0)) == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        // Null-terminate received data to print as string
        buffer[bytes_received] = '\0';
        printf("Received response from server: %s\n", buffer);
    }
        // Close the socket
        close(sockfd);

    return 0;
}
