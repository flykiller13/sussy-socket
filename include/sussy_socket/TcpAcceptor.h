#ifndef SUSSY_SOCKET_SERVERSOCKET_H
#define SUSSY_SOCKET_SERVERSOCKET_H
#include "TcpSocket.h"
#include <string>


/**
 * Represents a TCP server socket that can listen for incoming connections.
 * Provides functionality for accepting connections and managing the socket lifecycle.
 */
class TcpAcceptor {
public:
  explicit TcpAcceptor(const std::string &port);

  // Copy semantics
  TcpAcceptor(const TcpAcceptor &other) = delete;
  TcpAcceptor &operator=(const TcpAcceptor &other) = delete;

  // Move semantics
  TcpAcceptor(TcpAcceptor &&other) noexcept; // Move constructor
  TcpAcceptor &operator=(TcpAcceptor &&other) noexcept;
  // Move assignment operator

  ~TcpAcceptor();

  /**
   * Retrieves the file descriptor associated with the server's listening socket.
   * This file descriptor is used to manage the underlying listening socket.
   *
   * @return The file descriptor of the listening socket.
   */
  [[nodiscard]] int get_listen_socket_fd() const;


  /**
   * Accepts an incoming connection request on the server socket.
   * Creates a new active socket to handle communication with the connected client.
   *
   * @return A Socket object representing the newly connected client.
   */
  [[nodiscard]] TcpSocket accept_connection() const;

private:
  int listen_socket_fd_;
  const size_t backlog_ = 10;

};


#endif //SUSSY_SOCKET_SERVERSOCKET_H