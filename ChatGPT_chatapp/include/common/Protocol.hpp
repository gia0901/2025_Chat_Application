#pragma once
#include <string>
#include <string_view>

// Text-based protocol, newline-terminated for MVP
// Command: client -> server:
//      REGISTER <id>\n
//      SEND <target_id> <message>\n
//
// Server -> client:
//      MSG <from_id> <message>\n
//      ERROR <text>\n

namespace proto {
    std::string make_register(std::string_view id);
    std::string make_send(std::string_view target, std::string_view message);
    std::string make_msg(std::string_view  from, std::string_view message);
    std::string make_error(std::string_view text);
}   // namespace proto