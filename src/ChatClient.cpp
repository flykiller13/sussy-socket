#include "ChatClient.h"

#include <iostream>
#include <format>
#include <thread>
#include <chrono>

using namespace std;

ChatClient::ChatClient(const string& ip, const string& port, const string& client_name) : socket_()
{
    // Retry connection loop
    while (socket_ == nullptr)
    {
        try
        {
            socket_ = new Socket(ip, port);
            cout << "Successfully connected to server!" << endl;
        }
        catch (const exception& e)
        {
            cout << format("Failed to connect to server: {}", e.what()) << endl;
            cout << format("Retrying in {} seconds...", retry_delay_seconds_) << endl;
            this_thread::sleep_for(chrono::seconds(retry_delay_seconds_));
        }
    }

    // Add client to the poll list
    pollfd client_pfd{};
    client_pfd.fd = socket_->get_socket_fd();
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
    client_name_ = client_name;
    socket_->send_int(client_name_.length());
    socket_->send_data(client_name_);
}

void ChatClient::run()
{
    while (true)
    {
        // Poll for events on the pfds. Will sleep until an event happens.
        int poll_count = poll(pfds.data(), pfds.size(), -1);
        if (poll_count == -1) {
            throw runtime_error("poll");
        }

        // Check if the client is ready to read
        if (pfds[0].revents & (POLLIN | POLLHUP))
        {
            // Handle message from server
            uint32_t msg_len = socket_->receive_int(); // receive message length
            string received_msg = socket_->receive_data(msg_len); // receive message
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
                socket_->send_int(msg.length());
                socket_->send_data(msg);
            }
        }
    }
}
