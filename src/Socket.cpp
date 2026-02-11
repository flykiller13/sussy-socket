
#include "Socket.h"

#include <cstring>           // For memset()
#include <stdexcept>         // For std::runtime_error
#include <sys/types.h>       // For socket types
#include <sys/socket.h>      // For socket(), connect(), send(), recv()
#include <netdb.h>           // For getaddrinfo(), struct addrinfo
#include <arpa/inet.h>       // For inet_ntop()
#include <cstdio>            // For printf(), perror()
#include "network/netutils.h" // For get_in_addr()
#include <format>
#include <iostream>

using namespace std;

Socket::Socket(string ip, string port)
{
    socket_fd_ = -1;
    int status;
    struct addrinfo hints;
    int sockopt_val = 1;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo *servinfo = nullptr;  // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    try
    {
        // Get address info
        if ((status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo)) != 0)
        {
            throw std::runtime_error(gai_strerror(status));
        }

        // servinfo now points to a linked list of 1 or more struct addrinfos
        // loop through all the results and bind to the first we can
        const struct addrinfo *p; // will point to the results
        for(p = servinfo; p != nullptr; p = p->ai_next) {

            // Create socket
            if ((socket_fd_ = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol)) == -1) {
                cout << "socket creation failed. trying next address..." << endl;
                continue;
                    }

            inet_ntop(p->ai_family,
            sussy_socket::net::get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
            cout << "client: attempting connection to " << s << endl;

            if (connect(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
                close(socket_fd_);
                cout << "connection failed. trying next address..." << endl;
                continue;
            }

            break;
        }

        if (p == nullptr)
        {
            throw std::runtime_error("failed to connect");
        }

        inet_ntop(p->ai_family,
                sussy_socket::net::get_in_addr((struct sockaddr *)p->ai_addr),
                s, sizeof s);
        cout << "client: connected to " << s << endl;

        freeaddrinfo(servinfo); // done with this address
        servinfo = nullptr;
    }
    catch (...)
    {
        // Cleanup
        if (socket_fd_ != -1)
        {
            close(socket_fd_);
            socket_fd_ = -1;
        }
        if (servinfo != nullptr)
        {
            freeaddrinfo(servinfo);
            servinfo = nullptr;
        }

        throw;
    }
}

Socket::~Socket()
{
    if (socket_fd_ != -1)
        close(socket_fd_);
}

void Socket::send_data(const string& data)
{
    int bytes_sent, total_sent = 0;
    int len = data.length();

    while ( total_sent < len)
    {
        bytes_sent = send(socket_fd_, data.c_str() + total_sent, len - total_sent, 0);
        if (bytes_sent == -1)
        {
            throw std::runtime_error("send data failed");
        }
        total_sent += bytes_sent;
    }
}

string Socket::receive_data()
{
    int numbytes;
    const int MAXDATASIZE = 1024;
    char buf[MAXDATASIZE];
    if ((numbytes = recv(socket_fd_, buf, MAXDATASIZE-1, 0)) == -1)
    {
        throw std::runtime_error("receive data failed");
    }
    if (numbytes == 0)
    {
        throw std::runtime_error("client: server closed the connection");
    }
    buf[numbytes] = '\0'; // null-terminate whatever we received and treat like a string
    cout << format("client: received '{}'\n", buf);

    return buf;
}
