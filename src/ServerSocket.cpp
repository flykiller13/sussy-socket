
#include "ServerSocket.h"
#include <cstring>           // For memset()
#include <stdexcept>         // For std::runtime_error
#include <iostream>          // For cout, cerr
#include <sys/types.h>       // For socket types
#include <sys/socket.h>      // For socket(), bind(), listen(), accept(), setsockopt()
#include <netdb.h>           // For getaddrinfo(), struct addrinfo
#include <arpa/inet.h>       // For inet_ntop(), INET6_ADDRSTRLEN
#include <unistd.h>          // For close()
#include <signal.h>          // For sigaction(), SIGCHLD
#include "network/netutils.h" // For sigchld_handler(), get_in_addr()

#define BACKLOG 10

using namespace std;

ServerSocket::ServerSocket(string port)
{
    listen_socket_fd_ = -1;
    int status;
    struct addrinfo hints;
    int sockopt_val = 1;
    struct addrinfo *servinfo = nullptr;  // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    try
    {
        // Get address info
        if ((status = getaddrinfo(nullptr, port.c_str(), &hints, &servinfo)) != 0) {
            throw std::runtime_error(gai_strerror(status));
        }

        // servinfo now points to a linked list of 1 or more struct addrinfos
        // loop through all the results and bind to the first we can
        const struct addrinfo *p; // will hold the result
        for(p = servinfo; p != nullptr; p = p->ai_next) {

            // Create socket
            if ((listen_socket_fd_ = socket(p->ai_family, p->ai_socktype,
                                            p->ai_protocol)) == -1) {
                cout << "socket creation failed. trying next address..." << endl;
                continue;
                                            }

            // Set socket options
            if (setsockopt(listen_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &sockopt_val,
                           sizeof(int)) == -1) {
                throw std::runtime_error("setsockopt failed");
                           }

            // Bind to the local address
            if (bind(listen_socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
                cout << "bind failed. trying next address..." << endl;
                close(listen_socket_fd_);
                continue;
            }

            break;
        }

        if (p == nullptr)
        {
            throw std::runtime_error("failed to bind");
        }

        if (listen(listen_socket_fd_, BACKLOG) == -1)
        {
            throw std::runtime_error("listen failed");
        }

        // reap all dead processes
        struct sigaction sa;
        sa.sa_handler = sussy_socket::net::sigchld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
            throw std::runtime_error("sigaction failed");
        }

        freeaddrinfo(servinfo); // done with this address
        servinfo = nullptr;
    }
    catch (std::runtime_error &e)
    {
        // Cleanup
        if (listen_socket_fd_ != -1)
        {
            close(listen_socket_fd_);
            listen_socket_fd_ = -1;
        }
        if (servinfo != nullptr)
        {
            freeaddrinfo(servinfo);
            servinfo = nullptr;
        }

        cerr << e.what() << endl;
        throw;
    }

}

ServerSocket::~ServerSocket()
{
    // Cleanup
    if (listen_socket_fd_ != -1)
    {
        close(listen_socket_fd_);
        listen_socket_fd_ = -1;
    }
}

Socket ServerSocket::accept_connection()
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    char s[INET6_ADDRSTRLEN];

    int new_fd = accept(listen_socket_fd_, (struct sockaddr *)&their_addr, &addr_size);

    if (new_fd == -1)
    {
        throw std::runtime_error("accept failed");
    }

    inet_ntop(their_addr.ss_family, sussy_socket::net::get_in_addr((struct sockaddr *)&their_addr), s, sizeof s );
    cout << "server: got connection from " << s << endl;

    return {new_fd};
}