cmake_minimum_required(VERSION 3.16)
project(ChatApp LANGUAGES CXX)

# C++ 20 -> std::string_view, chrono, v.v...
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bật Warning
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
endif()

# Thư mục chứa header
include_directories(${CMAKE_SOURCE_DIR}/include)

# Tạo thư viện chính: chat_core
add_library(chat_core
    src/SocketWrapper.cpp
    src/common/Protocol.cpp
    src/Server.cpp
    src/Client.cpp
)

# Liên kết thư viện posix thread với chat core
target_link_libraries(chat_core PRIVATE pthread)

# Tạo exe chat_server từ main_server.cpp và liên kết với thư viện chat_core
add_executable(chat_server src/main_server.cpp)
target_link_libraries(chat_server PRIVATE chat_core)

# Tạo exe chat_client từ main_client.cpp và liên kết với thư viện chat_core
add_executable(chat_client src/main_client.cpp)
target_link_libraries(chat_client PRIVATE chat_core)
