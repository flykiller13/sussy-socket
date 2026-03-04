#ifndef RON_SERVER_SERVERSOCKET_H
#define RON_SERVER_SERVERSOCKET_H
#include "Socket.h"
#include <string>


class ServerSocket {
public:
  explicit ServerSocket(const std::string &port);

  // Copy semantics
  ServerSocket(const ServerSocket &other) = delete;
  ServerSocket &operator=(const ServerSocket &other) = delete;

  // Move semantics
  ServerSocket(ServerSocket &&other) noexcept; // Move constructor
  ServerSocket &operator=(ServerSocket &&other) noexcept;
  // Move assignment operator

  ~ServerSocket();

  [[nodiscard]] int get_listen_socket_fd() const;

  [[nodiscard]] Socket accept_connection() const;

private:
  int listen_socket_fd_;
  const size_t backlog_ = 10;

};


#endif //RON_SERVER_SERVERSOCKET_H