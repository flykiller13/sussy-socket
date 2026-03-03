#include "sussy_socket/UdpSocket.h"
#include <cstring>           // For memset()
#include <stdexcept>         // For std::runtime_error
#include <sys/types.h>       // For socket types
#include <sys/socket.h>      // For socket(), connect(), send(), recv()
#include <netdb.h>           // For getaddrinfo(), struct addrinfo
#include <arpa/inet.h>       // For inet_ntop()
#include "sussy_socket/netutils.h" // For get_in_addr()
#include <format>
#include <iostream>

#define MAXBUFLEN 100

using namespace std;

// Talker
UdpSocket::UdpSocket() : socket_fd_(-1)
{
    // fprintf(stderr,"usage: talker hostname message\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM; // Datagram for UDP

    int status;
    struct addrinfo *servinfo = nullptr;  // will point to the results
    try
    {
        // Create socket - connectionless
        if ((socket_fd_ = socket(AF_INET6, SOCK_DGRAM,
                    0)) == -1) {
            cout << "Failed to create socket..." << endl;
                    }


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

// Listener
UdpSocket::UdpSocket(std::string port) : socket_fd_(-1)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM; // Datagram for UDP
    hints.ai_flags = AI_PASSIVE; // use my IP

    int status;
    struct addrinfo *servinfo = nullptr;  // will point to the results
    try
    {
        // Get address info
        // WHATS THE IP:PORT?
        if ((status = getaddrinfo(nullptr, port.c_str(), &hints, &servinfo)) != 0)
        {
            throw std::runtime_error(gai_strerror(status));
        }

        // loop through all the results and make a socket
        // servinfo now points to a linked list of 1 or more struct addrinfos
        const struct addrinfo *p; // will point to the results
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((socket_fd_ = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol)) == -1) {
                cout << "Listener socket creation failed. trying next address..." << endl;
                continue;
                    }

            if (bind(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
                cout << "Listener bind failed. trying next address..." << endl;
                close(socket_fd_);
                continue;
            }

            break;
        }

        if (p == nullptr)
        {
            throw std::runtime_error("failed to bind listener socket");
        }

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

UdpSocket::UdpSocket(UdpSocket&& other) noexcept
{
    other.socket_fd_ = -1;
}

UdpSocket& UdpSocket::operator=(UdpSocket&& other) noexcept
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

UdpSocket::~UdpSocket()
{
    if (socket_fd_ != -1)
        close(socket_fd_);
}

void UdpSocket::send_to(const std::string& data, std::string& ip, std::string& port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM; // Datagram for UDP

    int status;
    struct addrinfo *servinfo = nullptr;  // will point to the results

    if ((status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo)) != 0)
    {
        throw std::runtime_error(gai_strerror(status));
    }

    // Loop through all the results and send to the first valid one
    int numbytes = -1;
    for(const struct addrinfo *p = servinfo; p != nullptr; p = p->ai_next) {
        if ((numbytes = sendto(socket_fd_, data.c_str(), data.size(), 0,
             p->ai_addr, p->ai_addrlen)) != -1) {
                cout << "talker: sent " << numbytes << " bytes to " << ip << endl;
                break;
             }
    }

    freeaddrinfo(servinfo); // done with this address
    servinfo = nullptr;

    if (numbytes == -1)
    {
        throw std::runtime_error("sendto failed");
    }
}

std::string UdpSocket::receive_from(std::string& ip, std::string& port)
{
    char bufffer[MAXBUFLEN];
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    int bytes_received;

    addr_len = sizeof their_addr;
    if ((bytes_received = recvfrom(socket_fd_, bufffer, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
        }

    printf("listener: got packet from %s\n",
        inet_ntop(their_addr.ss_family,
                  sussy_socket::net::get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s));
    printf("listener: packet is %d bytes long\n", bytes_received);
    bufffer[bytes_received] = '\0';
    printf("listener: packet contains \"%s\"\n", bufffer);
}
