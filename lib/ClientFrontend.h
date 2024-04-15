#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Input.H>
#include <string>
#include <arpa/inet.h>

class ClientFrontend {
public:
    ClientFrontend(int port, const std::string& host);
    ~ClientFrontend();

    void run(); // Starts the FLTK main loop

private:
    Fl_Window* window;
    Fl_Text_Display* textDisplay;
    Fl_Text_Buffer* textBuffer;
    Fl_Input* input;
    int sock;
    struct sockaddr_in serv_addr;

    static void send_message(Fl_Widget*, void* userdata);
    void connectToServer(int port, const std::string& host);
    void closeConnection();
    static void window_callback(Fl_Widget *w, void* userdata);
};