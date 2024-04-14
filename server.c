#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> // Include pthread library for threading
#include <stdlib.h> // Include stdlib library for malloc and free

#define PORT 8080
#define BUFFER_SIZE 1024

void *handle_connection(void *socket_ptr) {
    int socket = *((int*)socket_ptr);
    free(socket_ptr); // Free the heap-allocated memory for the socket descriptor

    char buffer[BUFFER_SIZE];
    int bytes_read = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytes_read < 0) {
        perror("Error reading from socket");
        close(socket);
        return NULL;
    }

    buffer[bytes_read] = '\0';
    printf("Received: %s\n", buffer);

    if (strncmp("GET ", buffer, 4) == 0) {
        char response[] =
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/html\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><h1>Hello, World!</h1></body></html>\r\n";

        int bytes_sent = send(socket, response, strlen(response), 0);
        if (bytes_sent < 0) {
            perror("Error sending response");
        }
        close(socket);
    }

    return NULL;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("Error setting socket options");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        return 1;
    }

    if (listen(sockfd, 10) < 0) {
        perror("Error listening on socket");
        return 1;
    }

    while (1) {
        int *new_socket = malloc(sizeof(int)); // Allocate memory for the socket descriptor
        if ((*new_socket = accept(sockfd, NULL, NULL)) < 0) {
            perror("Error accepting connection");
            free(new_socket); // Free allocated memory if accept fails
            continue;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_connection, new_socket) != 0) {
            perror("Failed to create thread");
            close(*new_socket);
            free(new_socket);
        }
        pthread_detach(thread_id); // Detach the thread to free resources after it finishes
    }

    return 0;
}