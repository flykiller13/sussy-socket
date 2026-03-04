#ifndef SUSSY_SOCKET_UDPSOCKET_H
#define SUSSY_SOCKET_UDPSOCKET_H
#include <cstdint> // For uint8_t
#include <string>
#include <vector>

class UdpSocket {
public:
  // constructor for talker
  UdpSocket();
  // constructor for listener
  explicit UdpSocket(const std::string &port);

  // Move semantics
  UdpSocket(UdpSocket &&other) noexcept; // Move constructor
  UdpSocket &operator=(UdpSocket &&other) noexcept; // Move assignment operator

  // Copying is prohibited
  UdpSocket(const UdpSocket &other) = delete; // Copy constructor
  UdpSocket &operator=(const UdpSocket &other) = delete;
  // Copy assignment operator

  ~UdpSocket();

  [[nodiscard]] int get_socket_fd() const {
    return socket_fd_;
  }

  void send_to(const std::string &data, const std::string &ip,
               const std::string &port) const;
  std::vector<uint8_t> receive_from(std::string &ip,
                                    std::string &port) const;

private:
  int socket_fd_;
  const size_t maxbuflen_ = 2048; // Should be enough for any MTU
};


#endif //SUSSY_SOCKET_UDPSOCKET_H