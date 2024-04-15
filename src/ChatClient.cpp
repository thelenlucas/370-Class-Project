#include "ClientFrontend.h"

int main(int argc, char** argv) {
    ClientFrontend clientFrontend(8080, "127.0.0.1");
    clientFrontend.run();
    return 0;
}