#include <iostream>
#include <sys/socket.h>

#include "sussy_socket/Socket.h"

#define IP "127.0.0.1"
#define PORT "3490"

using namespace std;

int main() {
  Socket client = Socket(IP, PORT);
  cout << "Connected to server at " << IP << ":" << PORT << '\n';

  string msg = "Hello from client!";
  client.send_int(msg.length());
  client.send_data(msg);

  uint32_t server_msg_len = client.receive_int();
  vector<uint8_t> server_msg = client.receive_data(server_msg_len);
  string server_msg_str = string(server_msg.begin(), server_msg.end());
  cout << "Server sent: " + server_msg_str << '\n';

  return 0;
}