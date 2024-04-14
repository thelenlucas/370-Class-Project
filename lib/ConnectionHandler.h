#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

class ConnectionHandler {
public:
    explicit ConnectionHandler(int socket);
    void handle();

private:
    int socket;
    void sendResponse(const char* message);
};