#ifndef SUSSY_SOCKET_UDPSOCKET_H
#define SUSSY_SOCKET_UDPSOCKET_H
#include <string>

class UdpSocket
{
public:
    // constructor for talker
    UdpSocket();
    // constructor for listener
    UdpSocket(const std::string& port);

    // Move semantics
    UdpSocket(UdpSocket&& other) noexcept; // Move constructor
    UdpSocket& operator=(UdpSocket&& other) noexcept; // Move assignment operator

    // Copying is prohibited
    UdpSocket(const UdpSocket& other) = delete; // Copy constructor
    UdpSocket& operator=(const UdpSocket& other) = delete; // Copy assignment operator

    ~UdpSocket();

    int get_socket_fd() const { return socket_fd_; }

    void send_to(const std::string& data, const std::string& ip, const std::string& port);
    std::string receive_from(std::string& ip, std::string& port);

private:
    int socket_fd_;
};


#endif //SUSSY_SOCKET_UDPSOCKET_H