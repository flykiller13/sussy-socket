
#include "ServerSocket.h"

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0) {}

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

ServerSocket::ServerSocket(string port)
{
    listen_socket_fd_ = -1;
    int status;
    int sockopt_val = 1;
    struct addrinfo *servinfo = nullptr;  // will point to the results

    memset(&hints_, 0, sizeof hints_); // make sure the struct is empty
    hints_.ai_family = AF_INET;     // don't care IPv4 or IPv6
    hints_.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints_.ai_flags = AI_PASSIVE;     // fill in my IP for me

    try
    {
        // Get address info
        if ((status = getaddrinfo(nullptr, port.c_str(), &hints_, &servinfo)) != 0) {
            fprintf(stderr, "get addr info error: %s\n", gai_strerror(status));
            throw std::runtime_error(gai_strerror(status));
        }

        // servinfo now points to a linked list of 1 or more struct addrinfos
        // loop through all the results and bind to the first we can
        const struct addrinfo *p; // will hold the result
        for(p = servinfo; p != nullptr; p = p->ai_next) {

            // Create socket
            if ((listen_socket_fd_ = socket(p->ai_family, p->ai_socktype,
                                            p->ai_protocol)) == -1) {
                printf("socket creation failed");
                continue;
                                            }

            // Set socket options
            if (setsockopt(listen_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &sockopt_val,
                           sizeof(int)) == -1) {
                throw std::runtime_error("setsockopt failed");
                           }

            // Bind to the local address
            if (bind(listen_socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
                printf("bind failed");
                close(listen_socket_fd_);
                continue;
            }

            break;
        }

        if (p == nullptr)
        {
            throw std::runtime_error("failed to bind");
        }

        if (listen(listen_socket_fd_, BACKLOG) == -1)
        {
            throw std::runtime_error("listen failed");
        }

        // reap all dead processes
        struct sigaction sa;
        sa.sa_handler = sigchld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
            throw std::runtime_error("sigaction failed");
        }

        freeaddrinfo(servinfo); // done with this address
    }
    catch (std::runtime_error &e)
    {
        // Cleanup
        if (listen_socket_fd_ != -1) close(listen_socket_fd_);
        if (servinfo != nullptr) freeaddrinfo(servinfo);

        cerr << e.what() << endl;
        throw;
    }

}

ServerSocket::~ServerSocket()
{
    if (listen_socket_fd_ != -1)
        close(listen_socket_fd_);
}

Socket ServerSocket::accept_connection()
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    char s[INET6_ADDRSTRLEN];

    int new_fd = accept(listen_socket_fd_, (struct sockaddr *)&their_addr, &addr_size);

    if (new_fd == -1)
    {
        throw std::runtime_error("accept failed");
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s );
    printf("server: got connection from %s\n", s);
}