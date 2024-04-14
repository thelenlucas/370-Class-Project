#pragma once

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <pthread.h>
#include "ConnectionHandler.h"

class Server {
public:
    Server(int port);
    ~Server();
    void run();

private:
    int sockfd;
    int port;
    struct sockaddr_in addr;

    void setupAddress();
    bool createSocket();
    bool bindSocket();
    void listenAndAccept();
};