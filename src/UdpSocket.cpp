#include "sussy_socket/UdpSocket.h"
#include "sussy_socket/netutils.h" // For get_in_addr()
#include <arpa/inet.h>       // For inet_ntop()
#include <cstring>           // For memset()
#include <format>
#include <iostream>
#include <netdb.h>           // For getaddrinfo(), struct addrinfo
#include <stdexcept>         // For std::runtime_error
#include <sys/socket.h>      // For socket(), connect(), send(), recv()
#include <sys/types.h>       // For socket types
#include <vector>

using namespace std;

// Talker
UdpSocket::UdpSocket() : socket_fd_(-1) {
  try {
    // Create socket - connectionless
    socket_fd_ = socket(AF_UNSPEC, SOCK_DGRAM,
                        0);
    if (socket_fd_ == -1) {
      throw std::runtime_error("socket creation failed");
    }
  } catch (...) {
    // Cleanup
    if (socket_fd_ != -1) {
      close(socket_fd_);
      socket_fd_ = -1;
    }

    throw;
  }
}

// Listener
UdpSocket::UdpSocket(const std::string &port) : socket_fd_(-1) {
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM; // Datagram for UDP
  hints.ai_flags = AI_PASSIVE; // use my IP

  int status = 0;
  addrinfo *servinfo = nullptr; // will point to the results
  try {
    // Get address info
    // We use an empty ip and the given port
    status = getaddrinfo(nullptr, port.c_str(), &hints, &servinfo);
    if (status != 0) {
      throw std::runtime_error(gai_strerror(status));
    }

    // servinfo now points to a linked list of 1 or more addrinfos
    // loop through all the results and make a socket
    const addrinfo *p = nullptr;
    for (p = servinfo; p != nullptr; p = p->ai_next) {
      socket_fd_ = socket(p->ai_family, p->ai_socktype,
                          p->ai_protocol);
      if (socket_fd_ == -1) {
        cout << "Listener socket creation failed. trying next address...\n";
        continue;
      }

      if (bind(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
        cout << "Listener bind failed. trying next address...\n";
        close(socket_fd_);
        continue;
      }

      break;
    }

    if (p == nullptr) {
      throw std::runtime_error("failed to bind listener socket");
    }

    freeaddrinfo(servinfo); // done with this address
    servinfo = nullptr;
  } catch (...) {
    // Cleanup
    if (socket_fd_ != -1) {
      close(socket_fd_);
      socket_fd_ = -1;
    }
    if (servinfo != nullptr) {
      freeaddrinfo(servinfo);
      servinfo = nullptr;
    }

    throw;
  }
}

UdpSocket::UdpSocket(UdpSocket &&other) noexcept : socket_fd_(
    other.socket_fd_) {
  other.socket_fd_ = -1;
}

UdpSocket &UdpSocket::operator=(UdpSocket &&other) noexcept {
  if (this != &other) {
    if (socket_fd_ != -1) {
      close(socket_fd_); // Close our own socket
    }

    socket_fd_ = other.socket_fd_;
    other.socket_fd_ = -1;
  }
  return *this;
}

UdpSocket::~UdpSocket() {
  if (socket_fd_ != -1) {
    close(socket_fd_);
  }
}

void UdpSocket::send_to(const std::string &data, const std::string &ip,
                        const std::string &port) const {
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM; // Datagram for UDP

  addrinfo *address_info = nullptr; // will point to the results
  if (int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &address_info);
    status != 0) {
    throw std::runtime_error(gai_strerror(status));
  }

  // Loop through all the results and send to the first valid one
  int numbytes = -1;
  for (const addrinfo *p = address_info; p != nullptr; p = p->ai_next) {
    numbytes = sendto(socket_fd_, data.c_str(), data.size(), 0,
                      p->ai_addr, p->ai_addrlen);
    if (numbytes != -1) {
      cout << "talker: sent " << numbytes << " bytes to " << ip << '\n';
      break;
    }
  }

  freeaddrinfo(address_info); // done with this address
  address_info = nullptr;

  if (numbytes == -1) {
    throw std::runtime_error("sendto failed");
  }
}

vector<uint8_t> UdpSocket::receive_from(std::string &ip,
                                        std::string &port) const {
  vector<uint8_t> buffer(maxbuflen_); // Will hold the packet

  sockaddr_storage their_addr{}; // Client address
  socklen_t addr_len = sizeof their_addr;
  char ip_buffer[INET6_ADDRSTRLEN]; // For inet_ntop()

  int bytes_received = -1;
  bytes_received = recvfrom(socket_fd_, buffer.data(), buffer.size(), 0,
                            reinterpret_cast<sockaddr *>(&
                              their_addr), &addr_len);
  if (bytes_received == -1) {
    throw std::runtime_error("recvfrom failed");
  }

  // Extract ip
  ip = inet_ntop(their_addr.ss_family,
                 sussy_socket::net::get_in_addr(
                     reinterpret_cast<sockaddr *>(&their_addr)),
                 ip_buffer, sizeof ip_buffer);

  // Extract port
  if (their_addr.ss_family == AF_INET) {
    port = to_string(
        ntohs(reinterpret_cast<sockaddr_in *>(&their_addr)->sin_port));
  } else {
    port = to_string(
        ntohs(reinterpret_cast<sockaddr_in6 *>(&their_addr)->sin6_port));
  }

  buffer.resize(bytes_received); // Trim to size

  cout << "listener: got packet from " << ip << ":" << port << '\n';
  cout << "listener: packet is " << bytes_received << " bytes long\n";

  return buffer;
}