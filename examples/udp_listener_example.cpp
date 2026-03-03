/*
** listener.c -- a datagram sockets "server" demo
*/

#include "sussy_socket/UdpSocket.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ostream>

#define MAXBUFLEN 100

using namespace std;

int main(void)
{
  UdpSocket socket("4950");

  string ip = "127.0.0.1";
  string port = "4950";
  string received_message = socket.receive_from(ip, port);

  cout << "Received: " << received_message << endl;
}