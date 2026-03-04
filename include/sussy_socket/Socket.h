#ifndef RON_SERVER_SOCKET_H
#define RON_SERVER_SOCKET_H

#include <cstdint>
#include <string>
#include <vector>

class Socket {
  friend class ServerSocket;

public:
  Socket() : socket_fd_(-1) {
  }

  Socket(const std::string &ip, const std::string &port);

  // Copying a socket is prohibited
  Socket(const Socket &other) = delete; // Copy constructor
  Socket &operator=(const Socket &other) = delete; // Copy assignment operator

  Socket(Socket &&other) noexcept;
  Socket &operator=(Socket &&other) noexcept; // Move assignment operator

  ~Socket();

  [[nodiscard]] int get_socket_fd() const {
    return socket_fd_;
  }

  void send_data(const std::string &data) const;
  [[nodiscard]] std::vector<uint8_t> receive_data(size_t num_bytes) const;

  void send_int(const uint32_t &data) const;
  [[nodiscard]] uint32_t receive_int() const;

private:
  explicit Socket(int socket_fd) : socket_fd_(socket_fd) {
  } // Private constructor for listener

  int socket_fd_;
};


#endif //RON_SERVER_SOCKET_H