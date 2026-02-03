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

// https://en.wikipedia.org/wiki/Berkeley_sockets#:~:text=the%20same%20computer.-,Socket%20API%20functions,-%5Bedit%5D

#define PORT "3490"
#define BACKLOG 10

using namespace std;

class Socket
{
public:
    Socket();
    Socket(string ip, string port);

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

    void send_data(const string& data);
    string receive_data();

private:
    int socket_fd_;
    int new_fd_;

    struct addrinfo hints;
    int MAXDATASIZE = 1024;
};


#endif //RON_SERVER_SOCKET_H