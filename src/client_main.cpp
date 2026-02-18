#include <format>
#include <iostream>
#include <vector>
#include <sys/poll.h>

#include "Socket.h"

#define IP "127.0.0.1"
#define PORT "3490"

using namespace std;

string get_client_name()
{
    string name;
    cout << "Enter username: ";
    getline(cin, name);
    cout << format("Welcome, {}!", name) << endl;
    return name;
}

int main()
{
    Socket client_socket = Socket(IP, PORT);
    string name = get_client_name();

    vector<pollfd> pfds;

    // Add client to the poll list
    pollfd client_pfd;
    client_pfd.fd = client_socket.get_socket_fd();
    client_pfd.events = POLLIN;
    client_pfd.revents = 0;
    pfds.push_back(client_pfd);

    // Add standard input to the poll list
    pollfd stdin_pfd;
    stdin_pfd.fd = STDIN_FILENO;
    stdin_pfd.events = POLLIN;
    stdin_pfd.revents = 0;
    pfds.push_back(stdin_pfd);

    // Send client name
    client_socket.send_data(name);

    while (true)
    {

        int poll_count = poll(pfds.data(), pfds.size(), -1);
        if (poll_count == -1) {
            throw runtime_error("poll");
        }

        // Check if the client is ready to read
        if (pfds[0].revents & (POLLIN | POLLHUP))
        {
            // Handle server read
            string received_msg = client_socket.receive_data();
            cout << received_msg << endl;
        }

        // Check if STDIN received events
        if (pfds[1].revents & (POLLIN))
        {
            string msg;
            cout << "You: ";
            getline(cin, msg);
            client_socket.send_data(msg);
        }
    }

    return 0;
}