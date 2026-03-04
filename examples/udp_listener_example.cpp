#include "sussy_socket/UdpSocket.h"

#include <iostream>

using namespace std;

int main(void) {
  UdpSocket socket("4950");

  string ip = "127.0.0.1";
  string port = "4950";
  vector<uint8_t> received_packet = socket.receive_from(ip, port);
  string received_message = string(received_packet.begin(),
                                   received_packet.end());

  cout << "Received: " << received_message << '\n';
}