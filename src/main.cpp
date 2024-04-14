#include "Server.h"
#include <iostream>

int main() {
    // Define the port number for the server to listen on.
    // Ensure this port is free on your system or change it to a free one.
    int port = 8080;

    try {
        // Create a server instance on the specified port.
        Server server(port);

        // Start the server to listen for incoming connections and handle clients.
        std::cout << "Starting the server on port " << port << std::endl;
        server.start();

        // The server is now set up and running in its own handling loop.
        // It will continue running indefinitely until an interrupt signal is received.
    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}