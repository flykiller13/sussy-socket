
#include "Socket.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Socket::Socket(string ip, string port)
{
    socket_fd_ = -1;
    int status;
    int sockopt_val = 1;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo *servinfo = nullptr;  // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    try
    {
        // Get address info
        if ((status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo)) != 0)
        {
            fprintf(stderr, "get addr info error: %s\n", gai_strerror(status));
            throw std::runtime_error(gai_strerror(status));
        }

        // servinfo now points to a linked list of 1 or more struct addrinfos
        // loop through all the results and bind to the first we can
        const struct addrinfo *p; // will point to the results
        for(p = servinfo; p != nullptr; p = p->ai_next) {

            // Create socket
            if ((socket_fd_ = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol)) == -1) {
                printf("socket creation failed");
                continue;
                    }

            inet_ntop(p->ai_family,
            get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
            printf("client: attempting connection to %s\n", s);

            if (connect(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
                close(socket_fd_);
                printf("connection failed");
                continue;
            }

            break;
        }

        if (p == nullptr)
        {
            throw std::runtime_error("failed to connect");
        }

        inet_ntop(p->ai_family,
                get_in_addr((struct sockaddr *)p->ai_addr),
                s, sizeof s);
        printf("client: connected to %s\n", s);

        freeaddrinfo(servinfo); // done with this address
    }
    catch (std::runtime_error &e)
    {
        // Cleanup
        if (socket_fd_ != -1) close(socket_fd_);
        if (servinfo != nullptr) freeaddrinfo(servinfo);

        cerr << e.what() << endl;
        throw;
    }
}

Socket::~Socket()
{
    if (socket_fd_ != -1)
        close(socket_fd_);
}

void Socket::send_data(const string& data)
{
    int bytes_sent, total_sent = 0;
    int len = data.length();

    while ( total_sent < len)
    {
        bytes_sent = send(new_fd_, data.c_str() + total_sent, len - total_sent, 0);
        if (bytes_sent == -1)
        {
            throw std::runtime_error("send data failed");
        }
        total_sent += bytes_sent;
    }
}

string Socket::receive_data()
{
    int numbytes;
    char buf[MAXDATASIZE];
    if ((numbytes = recv(new_fd_, buf, MAXDATASIZE-1, 0)) == -1)
    {
        throw std::runtime_error("receive data failed");
    }
    else if (numbytes == 0)
    {
        throw std::runtime_error("client: server closed the connection");
    }
    buf[numbytes] = '\0'; // null-terminate whatever we received and treat like a string
    printf("client: received '%s'", buf);

    return buf;
}
