#include "ClientHandler.h"

#include <iostream>

// Constructor
// Initializes the client handler with the client and server
// Parameters:
//    client - the client to handle
//    server - the server to handle the client for
ClientHandler::ClientHandler(Client* client, Server* server)
    : client(client), server(server), active(false) {

};

// Destructor
// Cleans up the client handler
// Mainly ensures the thread is stopped
ClientHandler::~ClientHandler() {
    stop();
    if (thread.joinable()) {
        thread.join();
    }
};

// Starts the client handler in a new thread
void ClientHandler::start() {
    active = true;
    thread = std::thread(&ClientHandler::run, this);
};

// Main loop for the client handler
// Handles incoming messages from the client
// and processes them
void ClientHandler::run() {
    while (active) {
        try {
            std::string message = client->receiveMessage();
            if (!message.empty()) {
                server->broadcastMessage(message);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break;
        }
    }

    // Cleanup on disconnect
    server->removeClient(this);
}

// Stops the client handler
void ClientHandler::stop() {
    active = false;
};