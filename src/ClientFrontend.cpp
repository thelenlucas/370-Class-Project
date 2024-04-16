#include "ClientFrontend.h"
#include <FL/Fl.H>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

ClientFrontend::ClientFrontend() : running(false) {  // Delay running
    window = new Fl_Window(640, 480, "Chat Client");
    textDisplay = new Fl_Text_Display(20, 20, 600, 350);
    textBuffer = new Fl_Text_Buffer();
    textDisplay->buffer(textBuffer);
    input = new Fl_Input(20, 380, 600, 25);
    input->callback(send_message, this);

    window->end();  // Setup main chat window but don't show it yet

    showLoginWindow();  // First, show the login window
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

void ClientFrontend::showLoginWindow() {
    loginWindow = new Fl_Window(300, 170, "Login");
    usernameInput = new Fl_Input(110, 30, 160, 30, "Username:");
    serverAddressInput = new Fl_Input(110, 70, 160, 30, "Server Address:");
    serverAddressInput->value("127.0.0.1");  // Default server address
    loginButton = new Fl_Button(100, 120, 100, 30, "Login");
    loginButton->callback(login_cb, this);

    loginWindow->end();
    loginWindow->show();
}

void ClientFrontend::login_cb(Fl_Widget*, void* userdata) {
    ClientFrontend* frontend = static_cast<ClientFrontend*>(userdata);
    frontend->username = frontend->usernameInput->value();
    frontend->serverAddress = frontend->serverAddressInput->value(); // Get server address from input

    // Use the server address and port provided by user (default port 8080 if not specified)
    frontend->connectToServer(port, frontend->serverAddress);
    frontend->window->show();
    frontend->loginWindow->hide();
    frontend->running = true;
    frontend->listenThread = std::thread(&ClientFrontend::listenForMessages, frontend);
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

    // Send a message to the server to identify the client
    std::string command = ".join " + username;
    if (send(sock, command.c_str(), command.length(), 0) < 0) {
        std::cerr << "Failed to send join message" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ClientFrontend::listenForMessages() {
    while (running) {
        char buffer[1024] = {0};
        int bytesReceived = recv(sock, buffer, 1024, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            Fl::lock();
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
    std::string message = frontend->username + ": " + frontend->input->value();
    if (send(frontend->sock, message.c_str(), message.length(), 0) < 0) {
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
    // Send .exit message to server
    std::string command = ".exit " + username;
    if (send(sock, command.c_str(), command.length(), 0) < 0) {
        std::cerr << "Failed to send exit message" << std::endl;
    }

    if (sock != 0) {
        close(sock);
        sock = 0;
    }
}