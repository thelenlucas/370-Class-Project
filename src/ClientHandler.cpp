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
    if (thread.joinable()) {
        if (std::this_thread::get_id() != thread.get_id()) {
            thread.join();  // Only join if not called from the same thread
        } else {
            thread.detach();  // Otherwise, detach the thread
        }
    }
}

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
            int receivedLength;
            std::string message = client->receiveMessage(receivedLength);
            
            if (receivedLength > 0) {
                std::cout << "Client Handler: Message received: " << message << std::endl;
                // Command messages start with a .
                if (message[0] == '.') {
                    // Command message
                    
                    // Exit command is ".exit username"
                    if (message.substr(0, 5) == ".exit") {
                        // Exit command, let other clients know this client is disconnecting
                        std::string username = message.substr(6);
                        server->broadcastMessage("Server: " + username + " disconnected");
                        continue;
                    }

                    // Join command is ".join username"
                    if (message.substr(0, 5) == ".join") {
                        // Join command, let other clients know this client is connecting
                        std::string username = message.substr(6);
                        server->broadcastMessage("Server: " + username + " connected");
                        continue;
                    }
                }
                
                // Otherwise, broadcast the message to all clients
                server->broadcastMessage(message);
            } else if (receivedLength == 0) {
                std::cout << "Client Handler: Client disconnected" << std::endl;
                break;
            } else {
                std::cerr << "Client Handler: Error receiving message" << std::endl;
                break;
            }

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break;
        }
    }

    // Cleanup on disconnect
    std::cout << "Client Handler: Stopping client handler" << std::endl;
    // Let the other clients know this client has disconnected
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