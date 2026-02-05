#ifndef RON_SERVER_SOCKET_H
#define RON_SERVER_SOCKET_H
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "network/netutils.h"

class Socket
{
    friend class ServerSocket;

public:
    Socket() : socket_fd_(-1) {}
    Socket(std::string ip, std::string port);

    ~Socket();

    // Copying a socket is prohibited
    Socket(const Socket& other) = delete; // Copy constructor
    Socket& operator=(const Socket& other) = delete; // Copy assignment operator

    Socket(Socket&& other) noexcept : socket_fd_(other.socket_fd_) // Move constructor
    {
        other.socket_fd_ = -1;
    }
    Socket& operator=(Socket&& other) noexcept // Move assignment operator
    {
        if (this != &other)
        {
            if (socket_fd_ != -1)
                close(socket_fd_); // Close our own socket

            socket_fd_ = other.socket_fd_;
            other.socket_fd_ = -1;
        }
        return *this;
    }

    void send_data(const std::string& data);
    std::string receive_data();

private:
    Socket(int socket_fd) : socket_fd_(socket_fd) {} // Private constructor for listener

    int socket_fd_;
};


#endif //RON_SERVER_SOCKET_H