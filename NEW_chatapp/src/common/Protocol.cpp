#include "common/Protocol.hpp"
#include <sstream>

namespace proto {
    std::string make_register(std::string_view id) {
        std::ostringstream oss;
        oss << "REGISTER " << id << '\n';
        return oss.str();
    }

    std::string make_send(std::string_view target, std::string_view message) {
        std::ostringstream oss;
        oss << "SEND" << target << ' ' << message << '\n';
        return oss.str();
    }

    std::string make_msg(std::string_view  from, std::string_view message) {
        std::ostringstream oss;
        oss << "MSG " << from << ' ' << message << '\n';
        return oss.str();
    }

    std::string make_error(std::string_view text) {
        std::ostringstream oss;
        oss << "ERROR " << text << '\n';
        return oss.str();
    }
}   // namespace proto