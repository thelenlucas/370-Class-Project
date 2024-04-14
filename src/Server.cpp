#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>

#include "Server.h"

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
    std::cout << "Server created on port " << port << std::endl;
}

// Destructor for Server
// Cleans up the resources used by the server
Server::~Server() {
    close(serverSocket);
    for (auto handler : clientHandlers) {
        delete handler;
    }
    std::cout << "Server destroyed" << std::endl;
}

// Configures the address structure for the server
// Uses simple AF_INET and INADDR_ANY for the address and port
void Server::setupAddress() {
    std::cout << "Server: Setting up address" << std::endl;
    // Zero out the address structure
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET; // IPv4
    addr.sin_addr.s_addr = INADDR_ANY; // Bind to any address
    addr.sin_port = htons(port); // Convert port to network byte order
    std::cout << "Server: Address set up" << std::endl;
}

// Creates the socket for the server
// Returns:
//    true if the socket was created successfully
//    false if the socket creation failed
bool Server::createSocket() {
    std::cout << "Server: Creating socket" << std::endl;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    std::cout << "Server: Socket created" << std::endl;
    return true;
}

// Binds the socket to the configured port
// Returns:
//    true if the socket was bound successfully
//    false if the socket binding failed
bool Server::bindSocket() {
    std::cout << "Server: Binding socket" << std::endl;
    if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    std::cout << "Server: Socket bound" << std::endl;
    return true;
}

// Starts the server's main loop
void Server::start() {
    std::cout << "Server: Starting" << std::endl;
    listenAndAccept();
}

// Listens for new connections and accepts them
// It spawns ClientHandler threads to handle the new connections
void Server::listenAndAccept() {
    // Listen for incoming connections
    std::cout << "Server: Listening for incoming connections" << std::endl;
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Server: Failed to listen on socket" << std::endl;
        return;
    }
    std::cout << "Server: Listening on socket" << std::endl;

    // Main loop to accept incoming connections
    while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            std::cerr << "Server: Failed to accept connection" << std::endl;
            continue;
        }
        std::cout << "Server: Accepted connection on socket " << clientSocket << std::endl;

        // Create a new client object
        std::cout << "Server: Creating client" << std::endl;
        Client* client = new Client(clientSocket, "Username"); // Need to implement username still
        std::cout << "Server: Client created" << std::endl;
        std::cout << "Server: Creating client handler" << std::endl;
        ClientHandler* handler = new ClientHandler(client, this);
        std::cout << "Server: Client handler created" << std::endl;
        std::cout << "Server: Adding client handler" << std::endl;
        addClient(handler);
        std::cout << "Server: Client handler added" << std::endl;
        std::cout << "Server: Starting client handler" << std::endl;
        handler->start();
        std::cout << "Server: Client handler started" << std::endl;
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