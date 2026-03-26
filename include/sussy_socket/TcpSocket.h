#ifndef SUSSY_SOCKET_SOCKET_H
#define SUSSY_SOCKET_SOCKET_H

#include <cstdint>
#include <string>
#include <vector>

/**
 * @class TcpSocket
 *
 * @brief Represents a network communication socket that facilitates data transmission
 *        and reception over TCP/IP.
 *
 * The `TcpSocket` class provides an abstraction for managing a network socket. It allows
 * establishing connections, sending data, and receiving data over a network connection.
 */
class TcpSocket {
  friend class TcpAcceptor;

public:
  TcpSocket() : socket_fd_(-1) {
  }

  TcpSocket(const std::string &ip, const std::string &port);

  // Copying a socket is prohibited
  TcpSocket(const TcpSocket &other) = delete; // Copy constructor
  TcpSocket &operator=(const TcpSocket &other) = delete;
  // Copy assignment operator

  TcpSocket(TcpSocket &&other) noexcept;
  TcpSocket &operator=(TcpSocket &&other) noexcept; // Move assignment operator

  ~TcpSocket();

  [[nodiscard]] int get_socket_fd() const {
    return socket_fd_;
  }

  void send_data(const std::vector<uint8_t> &data) const;
  [[nodiscard]] std::vector<uint8_t> receive_data() const;

private:
  explicit TcpSocket(int socket_fd) : socket_fd_(socket_fd) {
  } // Private constructor for listener

  int socket_fd_;
};


#endif //SUSSY_SOCKET_SOCKET_H