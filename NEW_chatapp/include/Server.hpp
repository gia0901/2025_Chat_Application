#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "SocketWrapper.hpp"


class ChatServer {
private:
    int port;
    Socket listener;
    bool running{true};

    struct ClientInfo {
        Socket sock;
        std::string id;
        std::string iBuff;
    };

    std::unordered_map<int, ClientInfo> clients;    // < fd, client_info >
    std::unordered_map<std::string, int> id_to_fd;  // < "id", fd >

    void setup_listener();
    void accept_new();
    void handle_client_read(int fd);
    void remove_client(int fd);

    void on_register(int fd, const std::string& id);
    void on_send(int from_fd, const std::string& target, const std::string& message);

    static std::vector<std::string> split_lines(std::string& buffer);

public:
    explicit ChatServer(int port);  // ????
    ~ChatServer();

    void run();
    void stop();
};