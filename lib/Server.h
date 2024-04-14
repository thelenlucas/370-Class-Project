#pragma once

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <pthread.h>
#include "Client.h"
#include "vector"
#include "string"
#include "ClientHandler.h"

class Server {
public:
    Server(int port);
    ~Server();
    void start();
    void acceptConnections();
    void broadcastMessage(const std::string& message);
    void run();

private:
    int serverSocket;
    std::vector<ClientHandler*> clientHandlers;
    int port;
    sockaddr_in addr;
    std::mutex clientsMutex;

    void setupAddress();
    bool createSocket();
    bool bindSocket();
    void listenAndAccept();
    void addClient(ClientHandler* handler);
    void removeClient(ClientHandler* handler);
};