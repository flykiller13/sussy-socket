#include <iostream>

#include "sussy_socket/ServerSocket.h"

#include <unistd.h>

#define PORT "3490"

using namespace std;

int main() {
  ServerSocket listen = ServerSocket(PORT);

  cout << "server: listening on port " << PORT << '\n';

  while (1) // main accept() loop
  {
    Socket new_socket = listen.accept_connection();
    if (fork() == 0) // this is the child process
    {
      vector<uint8_t> client_msg = new_socket.receive_data();
      string client_msg_str = string(client_msg.begin(), client_msg.end());
      cout << "Client sent: " + client_msg_str << '\n';

      string msg = "Hello from server!";
      vector<uint8_t> msg_data(msg.begin(), msg.end());
      new_socket.send_data(msg_data);
      exit(0);
    }
  }

  return 0;
}