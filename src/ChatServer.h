#ifndef RON_SERVER_CHATSERVER_H
#define RON_SERVER_CHATSERVER_H
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

#include "ServerSocket.h"

struct pollfd;

class ChatServer
{
public:
    ChatServer(std::string port) : listen_(port) { std::cout << "server: listening on port " << port << std::endl;}
    void run();

private:
    void handle_new_connection(std::vector<pollfd> &pfds);
    void handle_client_messages(std::vector<pollfd> &pfds, pollfd client_pfd);
    void add_to_pfds(std::vector<pollfd> &pfds, int socket_fd);
    void remove_from_pfds(std::vector<pollfd> &pfds, int i);
    void broadcast_message(std::string message, int sender_fd);

    ServerSocket listen_;
    std::vector<Socket> clients_;
    std::map<int, std::string> client_names_; // map of fd to name for each client
};


#endif //RON_SERVER_CHATSERVER_H