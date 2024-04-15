#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <string>
#include <arpa/inet.h>
#include <string>
#include <thread>
#include <atomic>

class ClientFrontend {
public:
    ClientFrontend();
    ~ClientFrontend();

    void run(); // Starts the FLTK main loop

void showLoginWindow();  // Show login window for username setup

private:
    Fl_Window* loginWindow;
    Fl_Input* usernameInput;
    Fl_Button* loginButton;

    std::string username;  // Store the username
    std::string serverAddress;  // Store the server address

    static void login_cb(Fl_Widget*, void* userdata);  // Callback for login button

    Fl_Window* window;
    Fl_Text_Display* textDisplay;
    Fl_Text_Buffer* textBuffer;
    Fl_Input* input;
    Fl_Input* serverAddressInput;

    int sock;
    static const int port = 8081;
    struct sockaddr_in serv_addr;
    std::thread listenThread;
    std::atomic<bool> running;

    static void send_message(Fl_Widget*, void* userdata);
    void listenForMessages();
    void connectToServer(int port, const std::string& host);
    void closeConnection();
    static void window_callback(Fl_Widget *w, void* userdata);
};