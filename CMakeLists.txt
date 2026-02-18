cmake_minimum_required(VERSION 3.28)
project(sussy_socket)

set(CMAKE_CXX_STANDARD 23)

add_executable(sussy_server
        src/Socket.cpp
        src/Socket.h
        src/ServerSocket.cpp
        src/ServerSocket.h
        src/ChatServer.cpp
        src/ChatServer.h
        src/chat_main.cpp
)

target_include_directories(sussy_server PRIVATE include/)

add_executable(sussy_client
        src/Socket.cpp
        src/Socket.h
        src/ServerSocket.cpp
        src/ServerSocket.h
        src/ChatServer.cpp
        src/ChatServer.h
        src/client_main.cpp
)

target_include_directories(sussy_client PRIVATE include/)