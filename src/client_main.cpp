#include "Socket.h"
#include "ServerSocket.h"

#define IP "127.0.0.1"
#define PORT "3490"

int main()
{
    Socket new_socket = Socket(IP, PORT);

    new_socket.receive_data();

    return 0;
}