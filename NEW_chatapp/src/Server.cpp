#include "Server.hpp"
#include "common/Protocol.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <sstream>

namespace {

}

ChatServer::ChatServer(int port) : port(port) {
    setup_listener();
}

ChatServer::~ChatServer() = default;

void ChatServer::setup_listener() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error("socket() failed");
    }
    //setsockopt

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(port));     // uint16_t htons(uint16_t hostshort);

    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        throw std::runtime_error("bind() failed");
    }

    if (::listen(fd, 128) < 0) {
        throw std::runtime_error("listen() failed");
    }

    listener.reset(fd);     // update listener with new fd
    std::cout << "[Server] Listening on port: " << port << '\n';
}

void ChatServer::run() {
    while (running) {
        fd_set readset;
        FD_ZERO(&readset);
        
        int maxfd = listener.fd();
        FD_SET(listener.fd(), &readset);    // add listener fd into readset

        // add all list of fd into readset
        for (auto& [fd, info] : clients) {
            FD_SET(fd, &readset);
            if (fd > maxfd)
                maxfd = fd;
        }

        int ret = ::select(maxfd + 1, &readset, nullptr, nullptr, nullptr); // fd starts from 0, so need to add 1 to get the number of fds.
        if (ret < 0) {
            if (errno == EINTR) // caused by Interrupt syscall, ignore it.
                continue;
            throw std::runtime_error("select() failed");
        }

        if (FD_ISSET(listener.fd(), &readset)) {
            accept_new();
        }

        // Push all 'set' fd into 'ready'
        std::vector<int> ready;
        ready.reserve(clients.size());
        for (auto& [fd,_] : clients) {
            if (FD_ISSET(fd, &readset)) {
                ready.push_back(fd);
            }
        }

        // Read all ready fds
        for (int fd : ready) {
            handle_client_read(fd);
        }
    }
}

void ChatServer::stop() {
    running = false;
}

void ChatServer::accept_new() {
    sockaddr_in cli{};
    socklen_t len = sizeof(cli);
    int client_fd = ::accept(listener.fd(), reinterpret_cast<sockaddr*>(&cli), &len);
    if (client_fd < 0) {
        std::cout << "[Server] failed to accept() new client!\n";
        return;
    }
    
    ClientInfo info;
    info.sock.reset(client_fd);
    info.id.clear();
    info.iBuff.clear();

    clients.emplace(client_fd, std::move(info));
    std::cout << "[Server] new connection accepted, fd: " << client_fd << '\n';
}

void ChatServer::handle_client_read(int fd) {
    char buff[4096];
    int n = recv(fd, buff, sizeof(buff), 0);
    if (n <= 0) {
        std::cout << "[Server] client fd: " << fd << " has disconnected.\n";
    }
    
    // store the message for the corresponding client
    ClientInfo& client = clients.at(fd);
    client.iBuff.append(buff, buff + n);

    // analyze each line and release the iBuff
    for (auto& line : split_lines(client.iBuff)) {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        /*  Example cmd:
            1: REGISTER <my_id>
            2: SEND <target_id> <message>
        */
        if (cmd == "REGISTER") {
            std::string id;
            iss >> id;
            on_register(fd, id);
        }
        else if (cmd == "SEND") {
            std::string target; iss >> target;
            std::string msg;    std::getline(iss, msg);
            
            if (!msg.empty() && msg.front() == ' ')
                msg.erase(0, 1);

            on_send(fd, target, msg);
        }
        else {
            std::string err = proto::make_error("Invalid command");
            ::send(fd, err.data(), err.size(), 0);  // raw sending to client
        }
    }
}

void ChatServer::on_register(int fd, const std::string& id) {
    if (!id.empty()) {
        std::string err = proto::make_error("Empty ID");
        ::send(fd, err.data(), err.size(), 0);
        return;
    }

    
}


std::vector<std::string> ChatServer::split_lines(std::string& buffer) {
    std::vector<std::string> lines;
    size_t pos = 0;
    for (;;) {
        size_t newline_pos = buffer.find('\n', pos);
        if (newline_pos == std::string::npos)
            break;
        
        lines.emplace_back(buffer.substr(pos, newline_pos-pos));
        pos = newline_pos + 1;
    }
    // Release the buffer (if there's something in there)
    if (pos > 0) {
        buffer.erase(0, pos);
    }
    return lines;
}
