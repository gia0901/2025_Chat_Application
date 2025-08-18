#include "Server.hpp"
#include <iostream>
#include <string>

const int default_port = 5555;

int main(int argc, char* argv[]) {
    int port = default_port;
    if (argc >= 2) {
        port = std::stoi(argv[1]);
    }

    try {
        ChatServer server(port);
        server.run();
    }
    catch (const std::exception& ex) {
        std::cerr << "[Server] ERROR: " << ex.what() << std::endl;
        return 1; 
    }

    return 0;
}