#include "sussy_socket/TcpSocket.h"

#include <iostream>

#define IP "127.0.0.1"
#define PORT "3490"

using namespace std;

int main() {
  TcpSocket client = TcpSocket(IP, PORT);
  cout << "Connected to server at " << IP << ":" << PORT << '\n';

  string msg = "Hello from client!";
  vector<uint8_t> msg_data(msg.begin(), msg.end());
  client.send_data(msg_data);

  vector<uint8_t> server_msg = client.receive_data();
  string server_msg_str = string(server_msg.begin(), server_msg.end());
  cout << "Server sent: " + server_msg_str << '\n';

  return 0;
}