#include "ChatServer.h"

#include <sys/poll.h>

using namespace std;

void ChatServer::run()
{
    vector<pollfd> pfds;
    // Add listener to the poll list
    pollfd listen_fd;
    listen_fd.fd = listen_.get_listen_socket_fd();
    listen_fd.events = POLLIN;
    listen_fd.revents = 0;
    pfds.push_back(listen_fd);

    while (true)
    {
        int poll_count = poll(pfds.data(), pfds.size(), -1);
        if (poll_count == -1) {
            throw runtime_error("poll");
        }

        // Run through connections looking for data to read
        for (int i = 0; i < pfds.size(); i++)
        {
            // Check if someone's ready to read
            if (pfds[i].revents & (POLLIN | POLLHUP))
            {
                // We got one!!
                if (pfds[i].fd == listen_.get_listen_socket_fd())
                {
                    // Handle new connection and add it to the list of clients
                    handle_new_connection(pfds);
                } else
                {
                    // Otherwise we're just a regular client
                    handle_client_messages(pfds, pfds[i]);
                }
            }
        }
    }
}

void ChatServer::handle_new_connection(vector<pollfd> &pfds)
{
    Socket new_socket = listen_.accept_connection();
    int new_fd = new_socket.get_socket_fd(); // since the ownership of the socket moves, we need to store the fd
    clients_.push_back(move(new_socket));
    add_to_pfds(pfds, new_fd);
}

void ChatServer::handle_client_messages(vector<pollfd> &pfds, pollfd client_pfd)
{
    try
    {
        for (Socket &client : clients_)
        {
            if (client.get_socket_fd() == client_pfd.fd)
            {
                uint32_t msg_len = client.receive_int(); // receive message length
                string msg = client.receive_data(msg_len);

                auto client_name = client_names_.find(client.get_socket_fd());
                if (client_name == client_names_.end()) // Is the client's name registered?
                {
                    client_names_[client.get_socket_fd()] = msg;
                    cout << msg << " has joined the chat!" << endl;
                } else
                {
                    // Client has a name, send the message to everyone!
                    msg = client_name->second + ": " + msg; // Prefix the sender's name
                    broadcast_message(msg, client.get_socket_fd());
                }
            }
        }
    }
    catch (...)
    {
        // Client disconnected
        for (int i = 0; i < clients_.size(); i++) // Erase from client sockets
        {
            if (clients_[i].get_socket_fd() == client_pfd.fd)
            {
                // Client connection is closed on removal
                client_names_.erase(clients_[i].get_socket_fd());
                clients_.erase(clients_.begin() + i);
                remove_from_pfds(pfds, i+1); // +1 for the listener
            }
        }
    }
}

/*
 * Add a new file descriptor to the set.
 */
void ChatServer::add_to_pfds(vector<pollfd> &pfds, int socket_fd)
{
    pollfd new_pfd;

    new_pfd.fd = socket_fd;
    new_pfd.events = POLLIN; // Check ready-to-read
    new_pfd.revents = 0;

    pfds.push_back(new_pfd);
}

/*
 * Remove a file descriptor at a given index from the set.
 */
void ChatServer::remove_from_pfds(vector<pollfd> &pfds, int i)
{
    pfds.erase(pfds.begin() + i);
}

void ChatServer::broadcast_message(string message, int sender_fd)
{
    for (Socket &client : clients_)
    {
        if (client.get_socket_fd() != sender_fd) // Dont send to sender
        {
            client.send_int(message.length()); // Send message length
            client.send_data(message); // send message itself
        }
    }
}
