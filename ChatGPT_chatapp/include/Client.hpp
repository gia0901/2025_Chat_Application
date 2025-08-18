#pragma once
#include "SocketWrapper.hpp"
#include <string>
#include <thread>
#include <atomic>

class ChatClient {
private:
    std::string server_ip;
    int port;
    Socket sock;
    std::atomic<bool> running{false};
    std::thread reader;

    void reader_loop();

public:
    ChatClient(std::string server_ip, int port);
    ~ChatClient();

    void connect_and_run();
    void send_raw_lines(const std::string& line);
    void register_id(const std::string& id);
    void send_to(const std::string& target, const std::string& message);

};
