cmake_minimum_required(VERSION 3.28)
project(sussy_socket)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the library
add_library(sussy_socket STATIC
        src/Socket.cpp
        src/ServerSocket.cpp
        src/UdpSocket.cpp
)

# Set include directories
target_include_directories(sussy_socket PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Optional: Build examples if this is the standalone repo
option(BUILD_EXAMPLES "Build example applications" ON)
if (BUILD_EXAMPLES)
    add_executable(socket_client_example examples/client_example.cpp)
    target_link_libraries(socket_client_example PRIVATE sussy_socket)

    add_executable(socket_server_example examples/server_example.cpp)
    target_link_libraries(socket_server_example PRIVATE sussy_socket)

    add_executable(udp_listener_example examples/udp_listener_example.cpp)
    target_link_libraries(udp_listener_example PRIVATE sussy_socket)

    add_executable(udp_talker_example examples/udp_talker_example.cpp)
    target_link_libraries(udp_talker_example PRIVATE sussy_socket)
endif ()