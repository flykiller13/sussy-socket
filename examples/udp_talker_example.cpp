#include "sussy_socket/UdpSocket.h"

using namespace std;

int main(int argc, char *argv[])
{
  UdpSocket talker = UdpSocket();

  string ip = "127.0.0.1";
  string port = "4950";
  string msg = "Hello from client!";

  talker.send_to(msg, ip, port);

  return 0;
}