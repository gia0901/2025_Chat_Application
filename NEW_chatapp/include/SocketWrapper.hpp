#pragma once
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <utility>

class Socket {
private:
    int fd;
    void close_current_fd() noexcept {
        if (fd > 0) {
            ::close(fd);
            fd = -1;
        }
    }

public:
    Socket() noexcept : fd(-1) {}
    Socket(const Socket&) = delete;                 // Non-copyable
    Socket(Socket&& rhs) noexcept : fd(rhs.fd) {
        rhs.fd = -1;                                // Make rhs's socket useless
    }
    Socket& operator=(Socket& rhs) noexcept {
        if (this != &rhs) {
            close_current_fd();
            fd = rhs.fd;
            rhs.fd = -1;
        }
        return *this;
    }
    ~Socket() {
        close_current_fd();
    }

    ////////////////// API //////////////////
    int fd() const noexcept {
        return fd;
    }
    bool valid() const noexcept {
        return fd >= 0;
    }
    void reset(int newfd = -1) {
        if (fd == newfd) return;
        close_current_fd();
        fd = newfd;
    }

};
