// Server.cpp
#include "Server.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

Server::Server(int port) : port(port), sockfd(-1) {
    createSocket();
    setupAddress();
}

Server::~Server() {
    if (sockfd != -1) {
        close(sockfd);
    }
}

void Server::setupAddress() {
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
}

bool Server::createSocket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        return false;
    }

    return true;
}

bool Server::bindSocket() {
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        return false;
    }
    return true;
}

void Server::listenAndAccept() {
    if (listen(sockfd, 10) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        return;
    }

    while (true) {
        int client_sock = accept(sockfd, NULL, NULL);
        if (client_sock < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        ConnectionHandler* handler = new ConnectionHandler(client_sock);
        pthread_t thread;
        pthread_create(&thread, NULL, [](void* arg) -> void* {
            ConnectionHandler* handler = static_cast<ConnectionHandler*>(arg);
            handler->handle();
            delete handler;
            return NULL;
        }, handler);
        pthread_detach(thread);
    }
}

void Server::run() {
    if (!bindSocket()) return;
    listenAndAccept();
}