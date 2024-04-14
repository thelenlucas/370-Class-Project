#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Input.H>

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Globals
Fl_Text_Display* textDisplay;
Fl_Text_Buffer* textBuffer;
Fl_Input* input;

int sock = 0;
struct sockaddr_in serv_addr;

void send_message(Fl_Widget*, void*) {
    const char* text = input->value();
    if (send(sock, text, strlen(text), 0) < 0) {
        std::cerr << "Failed to send message" << std::endl;
    }

    // Clear the input field
    input->value("");

    // Receive the response
    char buffer[1024] = {0};
    int bytesReceived = recv(sock, buffer, 1024, 0);
    if (bytesReceived < 0) {
        std::cerr << "Error in receiving response from server." << std::endl;
    } else {
        buffer[bytesReceived] = '\0'; // Null-terminate the received data
        textBuffer->append("Server: ");
        textBuffer->append(buffer);
        textBuffer->append("\n");
    }
}

int main(int argc, char **argv) {
    // Create and open the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    Fl_Window* window = new Fl_Window(640, 480, "Chat Client");
    textDisplay = new Fl_Text_Display(20, 20, 600, 350);
    textBuffer = new Fl_Text_Buffer();
    textDisplay->buffer(textBuffer);
    input = new Fl_Input(20, 380, 600, 25);
    input->callback(send_message);

    window->end();
    window->show(argc, argv);
    return Fl::run();
}