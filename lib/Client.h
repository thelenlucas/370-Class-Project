#pragma once

#include <string>

class Client {
public:
    Client(int socket, std::string username);
    std::string receiveMessage();
    void sendMessage(const std::string& message);

private:
    int socket;
    std::string username;

    void sendResponse(const char* message);
};