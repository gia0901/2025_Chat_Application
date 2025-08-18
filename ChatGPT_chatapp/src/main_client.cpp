#include "Client.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string server_ip = "127.0.0.1";
    int port = 5555;
    if (argc >= 2) server_ip = argv[1];
    if (argc >= 3) port = std::atoi(argv[2]);

    try {
        ChatClient c(server_ip, port);
        c.connect_and_run();

        std::cout << "Commands:\n"
                  << "REGISTER <my_id>\n"
                  << "SEND <target_id> <message>\n";

        std::string line;
        while (std::getline(std::cin, line)) {
            c.send_raw_lines(line);
        }
    }
    catch(const std::exception& ex) {
        std::cerr << "Client error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}