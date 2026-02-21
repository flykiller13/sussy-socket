#include <iostream>

#include "sussy_socket/ServerSocket.h"

#define PORT "3490"

using namespace std;

int main()
{
    ServerSocket listen = ServerSocket(PORT);

    cout << "server: listening on port " << PORT << endl;

    while(1) // main accept() loop
    {
        Socket new_socket = listen.accept_connection();
        if (!fork()) // this is the child process
        {
            uint32_t client_msg_len = new_socket.receive_int();
            cout << "Client sent: " + new_socket.receive_data(client_msg_len) << endl;

            string msg = "Hello from server!";
            new_socket.send_int(msg.length());
            new_socket.send_data(msg);
            exit(0);
        }
    }

    return 0;
}