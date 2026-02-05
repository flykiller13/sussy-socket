
#ifndef RON_SERVER_SERVERSOCKET_H
#define RON_SERVER_SERVERSOCKET_H
#include <string>
#include "Socket.h"


class ServerSocket
{
public:
    ServerSocket(std::string port);
    ~ServerSocket();

    Socket accept_connection();
private:
    int listen_socket_fd_;

};


#endif //RON_SERVER_SERVERSOCKET_H