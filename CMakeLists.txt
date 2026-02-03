cmake_minimum_required(VERSION 3.28)
project(ron_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(ron_server src/stream_server.cpp
        src/ServerSocket.cpp
        src/ServerSocket.h)
add_executable(ron_client src/stream_client.cpp
        src/Socket.cpp
        src/Socket.h
        src/ServerSocket.cpp
        src/ServerSocket.h)
