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

/**
 * @brief    Set Re-use address cho Server socket
 * @details  SO_REUSEADDR cho phép một socket tái sử dụng địa chỉ mà nó đã được liên kết trước đó. 
 *           Điều này rất hữu ích trong các tình huống sau:
 *              - Khởi động lại máy chủ: Nếu máy chủ bị dừng và khởi động lại ngay lập tức, 
 *                có thể có một khoảng thời gian mà địa chỉ IP và cổng vẫn đang được giữ bởi hệ thống. 
 *                Nếu không có SO_REUSEADDR, việc khởi động lại máy chủ sẽ gặp lỗi vì địa chỉ đã được sử dụng.
 *              - Nhiều socket trên cùng một địa chỉ: Trong một số trường hợp, bạn có thể muốn nhiều socket 
 *                lắng nghe trên cùng một địa chỉ và cổng (ví dụ: trong các ứng dụng multicast).
 * 
 * @note     được viết trong namespace vô danh -> chỉ có thể truy cập ở file này
 * @retval   Fail: < 0 - Success: 0
 * 
 */
namespace {
    int set_reuseaddr(int fd) {
        int opt = 1;  // bật tùy chọn
        return ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }
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
    // Set re-use address cho server
    if (set_reuseaddr(fd) < 0) {
        throw std::runtime_error("setsockopt(...SO_REUSEADDR...) failed");
    }

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

    listener.reset(fd);     // update fd vừa setup vào listener
    std::cout << "[Server] Listening on port: " << port << '\n';
}

/**
 * @brief   Khởi chạy Server
 * @details 
 * @retval  None
 * 
 */
void ChatServer::run() {
    while (running) {
        // Khởi tạo readset, gồm: listener_fd và các client fds
        fd_set readset;
        FD_ZERO(&readset);
        
        int maxfd = listener.get_fd();
        FD_SET(listener.get_fd(), &readset);

        // thêm tất cả client fd hiện tại vào readset
        for (auto& [fd, info] : clients) {
            FD_SET(fd, &readset);
            if (fd > maxfd)
                maxfd = fd;
        }

        int ret = ::select(maxfd + 1, &readset, nullptr, nullptr, nullptr); // fd đánh dấu từ 0, nên cộng 1 để ra số lượng fd.
        if (ret < 0) {
            if (errno == EINTR) // nếu select() fail vì interrupt, vẫn cho phép chạy tiếp
                continue;
            throw std::runtime_error("select() failed");
        }

        if (FD_ISSET(listener.get_fd(), &readset)) {
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

/**
 * @brief   Accept client mới
 * @details - Tạo 1 socket fd cho client mới
 *          - Update vào 'clients' map
 * @note    Client phải tự REGISTER id để có thể request server sau này
 * @retval  None
 * 
 */
void ChatServer::accept_new() {
    sockaddr_in cli{};
    socklen_t len = sizeof(cli);
    int client_fd = ::accept(listener.get_fd(), reinterpret_cast<sockaddr*>(&cli), &len);
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
    ssize_t n = ::recv(fd, buff, sizeof(buff), 0);
    if (n <= 0) {
        std::cout << "[Server] client fd: " << fd << " has disconnected.\n";
    }
    
    // store the message for the corresponding client
    ClientInfo& client = clients.at(fd);
    client.iBuff.append(buff, buff + n);

    // analyze each line and release the iBuff
    for (auto& line : split_lines(client.iBuff)) {
        std::cout << "[Debug] line: " << line << std::endl;
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
            std::cout << "[Debug] id: " << id << std::endl;
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

/**
 * @brief   Remove client
 * @details Cần phải xóa client ở 2 nơi: clients map và id_to_fd map
 * @retval  None
 * 
 */
void ChatServer::remove_client(int fd) {
    auto it = clients.find(fd);
    if (it != clients.end()) {
        if (!it->second.id.empty()) {
            id_to_fd.erase(it->second.id);
        }
        clients.erase(it);
    }
    ::close(fd); // Socket wrapper sẽ close ở destructor, nhưng đảm bảo close ngay
}

/**
 * @brief   Đăng ký ID cho một socket fd
 * @details 
 * @retval  None
 * 
 */
void ChatServer::on_register(int fd, const std::string& id) {
    if (id.empty()) {
        std::string err = proto::make_error("Empty ID");
        ::send(fd, err.data(), err.size(), 0);
        return;
    }

    auto existed = id_to_fd.find(id);
    // Có tồn tại 'id' trong map, nhưng có 1 fd khác -> remove fd cũ trước khi đăng ký fd mới cho id này
    if (existed != id_to_fd.end() && existed->second != fd) {
        int oldfd = existed->second;
        std::string info = proto::make_error("ID taken by new session, closing old fd.");
        ::send(oldfd, info.data(), info.size(), 0);
        remove_client(oldfd);
    }

    // Đăng ký fd mới cho id hiện tại
    clients.at(fd).id = id;
    id_to_fd[id] = fd;
    std::cout << "[Server] Registered fd: " << fd << " as '" << id << "'\n";
}

/**
 * @brief   Gửi message tới target theo yêu cầu của client (from_fd)
 * @details 
 * @retval  None
 * 
 */
void ChatServer::on_send(int from_fd, const std::string& target, const std::string& message) {
    // tìm from fd
    auto from_it = clients.find(from_fd);
    if (from_it == clients.end() || from_it->second.id.empty()) {
        std::string err = proto::make_error("You must REGISTER first");
        ::send(from_fd, err.data(), err.size(), 0);
        return;
    }

    // tìm target fd (bằng target id)
    auto target_it = id_to_fd.find(target);
    if (target_it == id_to_fd.end()) {
        std::string err = proto::make_error("Target not found");
        ::send(from_fd, err.data(), err.size(), 0);
        return;
    }

    // đã có fd hợp lệ, bắt đầu gửi message
    const std::string& from_id = from_it->second.id;
    std::string out = proto::make_msg(from_id, message);
    ::send(target_it->second, out.data(), out.size(), 0);
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
