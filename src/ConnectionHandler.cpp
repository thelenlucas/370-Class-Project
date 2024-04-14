// ConnectionHandler.cpp
#include "ConnectionHandler.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <string>

ConnectionHandler::ConnectionHandler(int socket) : socket(socket) {}

void ConnectionHandler::handle() {
    char buffer[1024] = {0};
    int bytes_read = recv(socket, buffer, 1024, 0);
    if (bytes_read < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        close(socket);
        return;
    }

    buffer[bytes_read] = '\0';
    std::cout << "Received: " << buffer << std::endl;

    if (strncmp("GET ", buffer, 4) == 0) {
        const char* response =
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/html\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><h1>Hello, World!</h1></body></html>\r\n";
        sendResponse(response);
    }
    close(socket);
}

void ConnectionHandler::sendResponse(const char* message) {
    int bytes_sent = send(socket, message, strlen(message), 0);
    if (bytes_sent < 0) {
        std::cerr << "Error sending response" << std::endl;
    }
}
