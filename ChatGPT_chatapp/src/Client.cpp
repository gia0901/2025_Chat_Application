#include "Client.hpp"
#include "common/Protocol.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

ChatClient::ChatClient(std::string server_ip, int port) 
    : server_ip(std::move(server_ip)), port(port) {}

ChatClient::~ChatClient() {
    running = false;
    if (reader.joinable()) {
        reader.join();
    }
}

void ChatClient::connect_and_run() {
    // Create socket to talk with server
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error("socket() failed");
    }

    // Prepare address info
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    if (::inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr) != 1) {
        throw std::runtime_error("inet_pton() failed");
    }

    // Connect to server
    if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        throw std::runtime_error("connect() failed");
    }

    sock.reset(fd);
    running = true;
    reader = std::thread(&ChatClient::reader_loop, this);
}

void ChatClient::reader_loop() {
    std::string iBuf;
    char buf[4096];
    // Read response from server
    while (running) {
        ssize_t n = ::recv(sock.get_fd(), buf, sizeof(buf), 0);
        if (n <= 0) {
            std::cout << "[Client] Disconnected from server.\n";
            running = false;
            break;
        }
        iBuf.append(buf, buf+n);
        for (;;) {
            auto pos = iBuf.find('\n');
            if (pos == std::string::npos)
                break;
            std::string line = iBuf.substr(0, pos);
            iBuf.erase(0, pos+1);
            std::cout << "[Server Repsonse] " << line << "\n";
        }
    }
}

/**
 * @brief   Gửi 1 raw string line tới server
 * 
 */
void ChatClient::send_raw_lines(const std::string& line) {
    std::string out = line;
    // Thêm '\n' nếu chuỗi trống hoặc thiếu '\n'
    if (out.empty() || out.back() != '\n') {
        out.push_back('\n');
    }
    std::cout << "[Debug] line: " << out << std::endl;
    ::send(sock.get_fd(), out.data(), out.size(), 0);
}

/**
 * @brief   Đăng ký ID với server
 * 
 */
void ChatClient::register_id(const std::string& id) {
    auto s = proto::make_register(id);
    ::send(sock.get_fd(), s.data(), s.size(), 0 );
}

/**
 * @brief   Request server gửi message tới target
 * 
 */
void ChatClient::send_to(const std::string& target, const std::string& message) {
    auto s = proto::make_send(target, message);
    ::send(sock.get_fd(), s.data(), s.size(), 0);
}