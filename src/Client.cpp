#include "Client.h"
#include "unistd.h"
#include <cstring>
#include <iostream>

// Constructor
// Parameters:
//   socket - the socket to use for the client
//   username - the username of the client
Client::Client(int socket, const std::string& username)
    : socket(socket), username(username) {
    std::cout << "Client: Client created" << std::endl;
};

// Receives a message from the client
// Returns:
//   the message received
std::string Client::receiveMessage() {
    std::cout << "Client: Receiving message" << std::endl;
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    int len = read(socket, buffer, sizeof(buffer));
    if (len < 0) {
        std::cerr << "Client: Socket read failed" << std::endl;
        return "";
    }

    std::cout << "Client: Message received" << std::endl;
    return std::string(buffer);
};

// Sends a message to the server
// Parameters:
//   message - the message to send
void Client::sendMessage(const std::string& message) {
    std::cout << "Client: Sending message" << std::endl;
    sendResponse(message.c_str());
    std::cout << "Client: Message sent" << std::endl;
};

// Sends a response to the server (helper function)
// Parameters:
//   message - the message to send
void Client::sendResponse(const char* message) {
    std::cout << "Client: Sending response" << std::endl;
    write(socket, message, strlen(message));
    std::cout << "Client: Response sent" << std::endl;
};