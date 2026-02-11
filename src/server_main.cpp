#include "ServerSocket.h"

#define PORT "3490"

int main()
{
    ServerSocket listen = ServerSocket(PORT);

    printf("server: waiting for connections...\n");

    while(1) // main accept() loop
    {
        Socket new_socket = listen.accept_connection();
        if (!fork()) // this is the child process
        {
            new_socket.send_data("Hello from server!");
            new_socket.receive_data();
            exit(0);
        }
    }

    return 0;
}