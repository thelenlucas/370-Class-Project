#pragma once

#include "thread"
#include "string"
#include "mutex"
#include "netinet/in.h"

#include "Client.h"
#include "Server.h"

#include <thread>
#include <atomic>

class ClientHandler {
public:
    explicit ClientHandler(Client* client, Server* server);
    ~ClientHandler();
    void start();
    void run();
    void stop();

private:
    Client* client;
    Server* server;
    std::thread thread;
    std::atomic<bool> active;

    void processMessages();
};
