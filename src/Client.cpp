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

};

// Receives a message from the client
// Returns:
//   the message received
std::string Client::receiveMessage() {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    int len = read(socket, buffer, sizeof(buffer));
    if (len < 0) {
        std::cerr << "Failed to read from socket" << std::endl;
        return "";
    }

    return std::string(buffer);
};

// Sends a message to the server
// Parameters:
//   message - the message to send
void Client::sendMessage(const std::string& message) {
    sendResponse(message.c_str());
};

// Sends a response to the server (helper function)
// Parameters:
//   message - the message to send
void Client::sendResponse(const char* message) {
    write(socket, message, strlen(message));
};