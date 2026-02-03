
#ifndef RON_SERVER_SERVERSOCKET_H
#define RON_SERVER_SERVERSOCKET_H
#include <string>
#include "Socket.h"

using namespace std;

class ServerSocket
{
public:
    ServerSocket(string port);
    ~ServerSocket();

    Socket accept_connection();
private:
    int listen_socket_fd_;
    struct addrinfo hints_;
};


#endif //RON_SERVER_SERVERSOCKET_H