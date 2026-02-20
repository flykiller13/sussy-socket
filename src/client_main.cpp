#include <format>
#include <iostream>
#include <vector>
#include <sys/poll.h>
#include <thread>
#include <chrono>

#include "Socket.h"

#define IP "127.0.0.1"
#define PORT "3490"
#define RETRY_DELAY_SECONDS 1

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
    Socket* client_socket = nullptr;
    
    // Retry connection loop
    while (client_socket == nullptr)
    {
        try
        {
            client_socket = new Socket(IP, PORT);
            cout << "Successfully connected to server!" << endl;
        }
        catch (const exception& e)
        {
            cout << format("Failed to connect to server: {}", e.what()) << endl;
            cout << format("Retrying in {} seconds...", RETRY_DELAY_SECONDS) << endl;
            this_thread::sleep_for(chrono::seconds(RETRY_DELAY_SECONDS));
        }
    }

    vector<pollfd> pfds;

    // Add client to the poll list
    pollfd client_pfd;
    client_pfd.fd = client_socket->get_socket_fd();
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
    string name = get_client_name();
    client_socket->send_int(name.length());
    client_socket->send_data(name);

    while (true)
    {

        int poll_count = poll(pfds.data(), pfds.size(), -1);
        if (poll_count == -1) {
            throw runtime_error("poll");
        }

        // Check if the client is ready to read
        if (pfds[0].revents & (POLLIN | POLLHUP))
        {
            // Handle message from server
            uint32_t msg_len = client_socket->receive_int(); // receive message length
            string received_msg = client_socket->receive_data(msg_len); // receive message
            cout << "\r\033[K" << received_msg << endl; // Clear the line
            cout << "You: " << flush; // Print received message
        }

        // Check if STDIN received events
        if (pfds[1].revents & (POLLIN))
        {
            string msg;
            cout << "You: ";
            getline(cin, msg);
            if (!msg.empty())
            {
                // server expects the length of the message first (4 bytes)
                // then the message itself
                client_socket->send_int(msg.length());
                client_socket->send_data(msg);
            }
        }
    }

    delete client_socket;
    return 0;
}