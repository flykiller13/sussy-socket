#ifndef SUSSY_SOCKET_CHATCLIENT_H
#define SUSSY_SOCKET_CHATCLIENT_H
#include <string>
#include <vector>
#include <sys/poll.h>

#include "Socket.h"


class ChatClient
{
public:
    ChatClient(const std::string& ip, const std::string& port, const std::string& client_name);
    void run();

private:
    Socket* socket_;
    std::string client_name_;
    std::vector<pollfd> pfds;
    int retry_delay_seconds_ = 1;
    int retry_count_ = 5;

};


#endif //SUSSY_SOCKET_CHATCLIENT_H