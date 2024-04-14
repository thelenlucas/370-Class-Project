#include "ClientHandler.h"

#include <iostream>

// Constructor
// Initializes the client handler with the client and server
// Parameters:
//    client - the client to handle
//    server - the server to handle the client for
ClientHandler::ClientHandler(Client* client, Server* server)
    : client(client), server(server), active(false) {
        std::cout << "Client Handler: Client handler created" << std::endl;
};

// Destructor
// Cleans up the client handler
// Mainly ensures the thread is stopped
ClientHandler::~ClientHandler() {
    std::cout << "Client Handler: Client handler destroying" << std::endl;
    stop();
    if (thread.joinable()) {
        thread.join();
    }
};

// Starts the client handler in a new thread
void ClientHandler::start() {
    std::cout << "Client Handler: Starting client handler" << std::endl;
    active = true;
    thread = std::thread(&ClientHandler::run, this);
    std::cout << "Client Handler: Client handler started" << std::endl;
};

// Main loop for the client handler
// Handles incoming messages from the client
// and processes them
void ClientHandler::run() {
    std::cout << "Client Handler: Running client handler main loop" << std::endl;
    while (active) {
        try {
            std::cout << "Client Handler: Receiving message" << std::endl;
            std::string message = client->receiveMessage();
            std::cout << "Client Handler: Message received" << std::endl;
            if (!message.empty()) {
                std::cout << "Client Handler: Broadcasting message" << std::endl;
                server->broadcastMessage(message);
                std::cout << "Client Handler: Message broadcasted" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break;
        }
    }

    // Cleanup on disconnect
    std::cout << "Client Handler: Stopping client handler" << std::endl;
    server->removeClient(this);
}

// Stops the client handler
void ClientHandler::stop() {
    active = false;
};

// Gets the client for the handler
// Returns:
//    the client
Client* ClientHandler::getClient() {
    return client;
};