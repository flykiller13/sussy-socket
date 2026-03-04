#include "../include/sussy_socket/ServerSocket.h"
#include <cstring>           // For memset()
#include <iostream>          // For cout, cerr
#include <stdexcept>         // For std::runtime_error
#include <sys/socket.h>      // For socket(), bind(), listen(), accept(), setsockopt()



#include <sys/types.h>       // For socket types

#include "sussy_socket/netutils.h" // For sigchld_handler(), get_in_addr()
#include <arpa/inet.h>       // For inet_ntop(), INET6_ADDRSTRLEN
#include <csignal>          // For sigaction(), SIGCHLD
#include <netdb.h>           // For getaddrinfo(), struct addrinfo
#include <unistd.h>          // For close()

using namespace std;

ServerSocket::ServerSocket(const string &port) : listen_socket_fd_(-1) {
  int status = 0;

  int sockopt_val = 1;
  addrinfo *addr_info = nullptr; // will point to the results

  addrinfo hints{};
  hints.ai_family = AF_INET; // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE; // fill in my IP for me

  try {
    // Get address info
    status = getaddrinfo(nullptr, port.c_str(), &hints, &addr_info);
    if (status != 0) {
      throw std::runtime_error(gai_strerror(status));
    }

    // addr_info now points to a linked list of 1 or more addrinfos
    // loop through all the results and bind to the first we can
    const addrinfo *p = nullptr; // will hold the result
    for (p = addr_info; p != nullptr; p = p->ai_next) {

      // Create socket
      listen_socket_fd_ = socket(p->ai_family, p->ai_socktype,
                                 p->ai_protocol);
      if (listen_socket_fd_ == -1) {
        cout << "socket creation failed. trying next address...\n";
        continue;
      }

      // Set socket options
      if (setsockopt(listen_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &sockopt_val,
                     sizeof(int)) == -1) {
        throw std::runtime_error("setsockopt failed");
      }

      // Bind to the local address
      if (bind(listen_socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
        cout << "bind failed. trying next address...\n";
        close(listen_socket_fd_);
        continue;
      }

      break;
    }

    if (p == nullptr) {
      throw std::runtime_error("failed to bind");
    }

    if (listen(listen_socket_fd_, backlog_) == -1) {
      throw std::runtime_error("listen failed");
    }

    // reap all dead processes
    struct sigaction sa{};
    sa.sa_handler = sussy_socket::net::sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
      throw std::runtime_error("sigaction failed");
    }

    freeaddrinfo(addr_info); // done with this address
    addr_info = nullptr;
  } catch (std::runtime_error &e) {
    // Cleanup
    if (listen_socket_fd_ != -1) {
      close(listen_socket_fd_);
      listen_socket_fd_ = -1;
    }
    if (addr_info != nullptr) {
      freeaddrinfo(addr_info);
      addr_info = nullptr;
    }

    throw;
  }
}

ServerSocket::ServerSocket(ServerSocket &&other) noexcept : listen_socket_fd_(
    other.listen_socket_fd_) {
  other.listen_socket_fd_ = -1;
}

ServerSocket &ServerSocket::operator=(ServerSocket &&other) noexcept {
  if (this != &other) {
    if (listen_socket_fd_ != -1) {
      close(listen_socket_fd_); // Close our own socket
    }

    listen_socket_fd_ = other.listen_socket_fd_;
    other.listen_socket_fd_ = -1;
  }
  return *this;
}

ServerSocket::~ServerSocket() {
  // Cleanup
  if (listen_socket_fd_ != -1) {
    close(listen_socket_fd_);
    listen_socket_fd_ = -1;
  }
}

int ServerSocket::get_listen_socket_fd() const {
  return listen_socket_fd_;
}

Socket ServerSocket::accept_connection() const {
  sockaddr_storage their_addr{};
  socklen_t addr_size = sizeof their_addr;
  char ip_buffer[INET6_ADDRSTRLEN];

  int new_fd = accept(listen_socket_fd_,
                      reinterpret_cast<sockaddr *>(&their_addr),
                      &addr_size);

  if (new_fd == -1) {
    throw std::runtime_error("accept failed");
  }

  inet_ntop(their_addr.ss_family,
            sussy_socket::net::get_in_addr(
                reinterpret_cast<sockaddr *>(&their_addr)), ip_buffer,
            sizeof ip_buffer);
  cout << "server: got connection from " << ip_buffer << '\n';

  return Socket(new_fd);
}