#include "sussy_socket/UdpSocket.h"

using namespace std;

int main() {
  UdpSocket talker = UdpSocket();

  string ip = "127.0.0.1";
  string port = "4950";

  string msg = "Hello from client!";
  vector<uint8_t> msg_data(msg.begin(), msg.end());

  talker.send_to(msg_data, ip, port);

  return 0;
}