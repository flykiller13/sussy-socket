
#include "sussy_socket/Socket.h"

#include "sussy_socket/netutils.h" // For get_in_addr()
#include <arpa/inet.h>       // For inet_ntop()
#include <iostream>
#include <netdb.h>           // For getaddrinfo(), struct addrinfo
#include <stdexcept>         // For std::runtime_error
#include <sys/socket.h>      // For socket(), connect(), send(), recv()

using namespace std;

Socket::Socket(const string &ip, const string &port) : socket_fd_(-1) {
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

  int status = 0;
  addrinfo *addr_info = nullptr; // will point to the results

  char ip_buffer[INET6_ADDRSTRLEN];
  try {
    // Get address info
    status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &addr_info);
    if (status != 0) {
      throw std::runtime_error(gai_strerror(status));
    }

    // addr_info now points to a linked list of 1 or more addrinfos
    // loop through all the results and bind to the first we can
    const addrinfo *p = nullptr; // will point to the results
    for (p = addr_info; p != nullptr; p = p->ai_next) {
      // Create socket
      socket_fd_ = socket(p->ai_family, p->ai_socktype,
                          p->ai_protocol);
      if (socket_fd_ == -1) {
        cout << "Socket creation failed. trying next address...\n";
        continue;
      }

      inet_ntop(p->ai_family,
                sussy_socket::net::get_in_addr((sockaddr *)p->ai_addr),
                ip_buffer, sizeof ip_buffer);
      cout << "client: attempting connection to " << ip_buffer << '\n';

      if (connect(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
        close(socket_fd_);
        cout << "connection failed. trying next address...\n";
        continue;
      }

      break;
    }

    if (p == nullptr) {
      throw std::runtime_error("failed to connect");
    }

    inet_ntop(p->ai_family,
              sussy_socket::net::get_in_addr((sockaddr *)p->ai_addr),
              ip_buffer, sizeof ip_buffer);

    freeaddrinfo(addr_info); // done with this address
    addr_info = nullptr;
  } catch (...) {
    // Cleanup
    if (socket_fd_ != -1) {
      close(socket_fd_);
      socket_fd_ = -1;
    }
    if (addr_info != nullptr) {
      freeaddrinfo(addr_info);
      addr_info = nullptr;
    }

    throw;
  }
}

Socket::~Socket() {
  if (socket_fd_ != -1) {
    close(socket_fd_);
  }
}

Socket::Socket(Socket &&other) noexcept : socket_fd_(other.socket_fd_) {
  other.socket_fd_ = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    if (socket_fd_ != -1) {
      close(socket_fd_); // Close our own socket
    }

    socket_fd_ = other.socket_fd_;
    other.socket_fd_ = -1;
  }
  return *this;
}

void Socket::send_data(const string &data) const {
  int bytes_sent = 0;
  size_t total_sent = 0;
  size_t len = data.length();

  while (total_sent < len) {
    bytes_sent = send(socket_fd_, data.c_str() + total_sent, len - total_sent,
                      0);
    if (bytes_sent == -1) {
      throw std::runtime_error("send data failed");
    }
    total_sent += bytes_sent;
  }
}

vector<uint8_t> Socket::receive_data(size_t num_bytes) const {
  int bytes_received = 0;
  size_t total_received = 0;
  vector<uint8_t> buf(num_bytes);

  // loop until we receive all the requested bytes
  while (total_received < num_bytes) {
    bytes_received =
        recv(socket_fd_, buf.data(), (num_bytes - total_received), 0);
    if (bytes_received == -1) {
      throw std::runtime_error("receive data failed");
    }
    if (bytes_received == 0) {
      throw std::runtime_error("client: server closed the connection");
    }
    total_received += bytes_received;
  }

  return buf;
}

void Socket::send_int(const uint32_t &data) const {
  uint32_t network_order = htonl(data); // convert to network byte order
  if (send(socket_fd_, &network_order, sizeof(uint32_t), 0) == -1) // send int
  {
    throw std::runtime_error("send data failed");
  }
}

uint32_t Socket::receive_int() const {
  uint32_t network_order = 0;
  if (recv(socket_fd_, &network_order, sizeof(int), 0) == -1) // receive int
  {
    throw std::runtime_error("receive data failed");
  }
  return ntohl(network_order);
}
