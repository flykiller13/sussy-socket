
#pragma once

#include <cerrno>        // For errno
#include <csignal>       // For SIGCHLD (though might not be needed if only used in impl)



#include <netinet/in.h>  // For sockaddr_in, sockaddr_in6
#include <sys/socket.h>  // For sockaddr, AF_INET, AF_INET6
#include <sys/wait.h>    // For waitpid(), WNOHANG

namespace sussy_socket::net {
inline void sigchld_handler(int s) {
  (void)s; // quiet unused variable warning

  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0) {
  }

  errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
inline void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
} // namespace sussy_socket::net