#include "Server.h"
#include "ClientHandler.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>

// Constructor for Server
// Initializes the server to listen on the specified port
// Parameters:
//    port - the port number to bind the server to
Server::Server(int port) : port(port), serverSocket(-1) {
    setupAddress();
    if(!createSocket() || !bindSocket()) {
        std::cerr << "Failed to create or bind socket" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Destructor for Server
// Cleans up the resources used by the server
Server::~Server() {
    close(serverSocket);
    for (auto handler : clientHandlers) {
        delete handler;
    }
}

// Configures the address structure for the server
// Uses simple AF_INET and INADDR_ANY for the address and port
void Server::setupAddress() {
    // Zero out the address structure
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET; // IPv4
    addr.sin_addr.s_addr = INADDR_ANY; // Bind to any address
    addr.sin_port = htons(port); // Convert port to network byte order
}

// Creates the socket for the server
// Returns:
//    true if the socket was created successfully
//    false if the socket creation failed
bool Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    return true;
}

// Binds the socket to the configured port
// Returns:
//    true if the socket was bound successfully
//    false if the socket binding failed
bool Server::bindSocket() {
    if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    return true;
}

// Starts the server's main loop
void Server::start() {
    listenAndAccept();
}

// Listens for new connections and accepts them
// It spawns ClientHandler threads to handle the new connections
void Server::listenAndAccept() {
    // Listen for incoming connections
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return;
    }

    std::cout << "Server started on port " << port << std::endl;

    // Main loop to accept incoming connections
    while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        // Create a new client object
        Client* client = new Client(clientSocket, "Username"); // Need to implement username still
        ClientHandler* handler = new ClientHandler(client, this);
        addClient(handler);
        handler->start();
    }
}

// Broadcasts a message to all connected clients
// Parameters:
//    message - the message to broadcast
void Server::broadcastMessage(const std::string& message) {
    // Lock the clients vector to use it safely
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto handler : clientHandlers) {
        handler->getClient()->sendMessage(message);
    }
}

// Adds a client handler to the server's list of clients
// Parameters:
//    handler - the client handler to add
void Server::addClient(ClientHandler* handler) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    clientHandlers.push_back(handler);
}

// Removes a client handler from the server's list of clients
// Parameters:
//    handler - the client handler to remove
void Server::removeClient(ClientHandler* handler) {
    std::lock_guard<std::mutex> guard(clientsMutex);
    auto it = std::find(clientHandlers.begin(), clientHandlers.end(), handler);
    if (it != clientHandlers.end()) {
        clientHandlers.erase(it);  // Correct usage of erase with an iterator
        delete handler;
    }
}