#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080 // HTTP uses this port by default
#define BUFFER_SIZE 1024

void handle_connection(int socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytes_read < 0) {
        perror("Error reading from socket");
        close(socket);
        return;
    }

    buffer[bytes_read] = '\0'; // We're in C, so make sure we append a null terminator
    printf("Received: %s\n", buffer);

    // GET req
    if (strncmp("GET ", buffer, 4) == 0) {
        char response[] = 
            "HTTP/1.1 200 OK\n" // Status line, contains the HTTP version and status code
            "Content-Type: text/html\n" // Content type header (we're using HTML)
            "Connection: close\r\n" // Close the connection after sending the response
            "\r\n" // CRLF (required to end the headers)
            "<html><body><h1>Hello, World!</h1></body></html>\r\n"; // Response body (very basic)
        
        int bytes_sent = send(socket, response, strlen(response), 0);

        if (bytes_sent < 0) {
            perror("Error sending response");
        }

        close(socket);
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // We're using IPv4 and TCP

    // Check if the socket was created successfully
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("Error setting socket options");
        return 1;
    }

    // Bind the socket to the address and port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        return 1;
    }

    // Listen for incoming connections (max 10)
    if (listen(sockfd, 10) < 0) {
        perror("Error listening on socket");
        return 1;
    }

    int new_socket;
    while (1) {
        if ((new_socket = accept(sockfd, NULL, NULL)) < 0) {
            perror("Error accepting connection");
            return 1;
        }

        handle_connection(new_socket);
    }

    return 0;
}