#pragma once

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <pthread.h>
#include "Client.h"
#include "vector"
#include "string"

class Server {
public:
    Server(int port);
    ~Server();
    void start();;
    void acceptConnections();
    void broadCastMessage(std::string message);
    void run();

private:
    int serverSocket;
    std::vector<Client*> clients;
    int port;
    sockaddr_in addr;

    void setupAddress();
    bool createSocket();
    bool bindSocket();
    void listenAndAccept();
};