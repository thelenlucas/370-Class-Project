#include "ClientFrontend.h"
#include <FL/Fl.H>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

ClientFrontend::ClientFrontend(int port, const std::string& host) : running(true) {
    connectToServer(port, host);

    window = new Fl_Window(640, 480, "Chat Client");
    textDisplay = new Fl_Text_Display(20, 20, 600, 350);
    textBuffer = new Fl_Text_Buffer();
    textDisplay->buffer(textBuffer);
    input = new Fl_Input(20, 380, 600, 25);
    input->callback(send_message, this);

    window->callback(window_callback, this);
    window->end();

    listenThread = std::thread(&ClientFrontend::listenForMessages, this);
}

ClientFrontend::~ClientFrontend() {
    running = false;
    if (listenThread.joinable()) {
        listenThread.join();
    }
    closeConnection();
    delete window;
    delete textDisplay;
    delete textBuffer;
    delete input;
}

void ClientFrontend::run() {
    window->show();
    Fl::run();
}

#include <fcntl.h> // For fcntl
#include <errno.h> // For errno

void ClientFrontend::connectToServer(int port, const std::string& host) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get socket flags" << std::endl;
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, flags) == -1) {
        std::cerr << "Failed to set socket to non-blocking" << std::endl;
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        if (errno != EINPROGRESS) {
            std::cerr << "Connection Failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void ClientFrontend::listenForMessages() {
    while (running) {
        char buffer[1024] = {0};
        int bytesReceived = recv(sock, buffer, 1024, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            Fl::lock();
            textBuffer->append("Server: ");
            textBuffer->append(buffer);
            textBuffer->append("\n");
            Fl::unlock();
            Fl::awake();
        } else if (bytesReceived == 0) {
            // Server closed connection
            break;
        } else {
            // No data received, or error occurred
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                break;  // An actual error occurred
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to prevent tight loop
        }
    }
}

void ClientFrontend::send_message(Fl_Widget*, void* userdata) {
    ClientFrontend* frontend = static_cast<ClientFrontend*>(userdata);
    const char* text = frontend->input->value();
    if (send(frontend->sock, text, strlen(text), 0) < 0) {
        std::cerr << "Failed to send message" << std::endl;
    }
    frontend->input->value("");
}

void ClientFrontend::window_callback(Fl_Widget *w, void* userdata) {
    ClientFrontend* frontend = static_cast<ClientFrontend*>(userdata);
    frontend->running = false;
    frontend->closeConnection();
    w->hide();
}

void ClientFrontend::closeConnection() {
    if (sock != 0) {
        close(sock);
        sock = 0;
    }
}