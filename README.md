# Sussy Socket

A utility networking library for implementing data transfer over TCP and UDP protocols. This C++ library provides a
clean, RAII-compliant wrapper around POSIX socket functionality with robust error handling, built around the principles
from [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split-wide/index.html).

## Table of Contents

- [Features](#features)
- [Socket Types](#socket-types)
- [Installation](#installation)
    - [Using CMake (Recommended)](#using-cmake-recommended)
    - [Adding to Existing CMake Project](#adding-to-existing-cmake-project)
    - [Manual Integration](#manual-integration)
- [Requirements](#requirements)
- [Running Examples](#running-examples)
    - [Build Examples](#build-examples)
    - [TCP Examples](#tcp-examples)
    - [UDP Examples](#udp-examples)
- [Quick Usage Example](#quick-usage-example)
    - [TCP Client](#tcp-client)
    - [UDP Communication](#udp-communication)
- [Build Options](#build-options)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Features

- **TCP Socket Support**: Full-featured TCP client and server socket implementations
- **UDP Socket Support**: Connectionless UDP communication for both sending and receiving
- **RAII Design**: Automatic resource management with proper cleanup
- **Error Handling**: Comprehensive error handling and exception safety
- **POSIX Compliance**: Built on standard POSIX socket APIs for cross-platform compatibility
- **Modern C++**: Utilizes C++23 features for clean, type-safe code

## Socket Types

- **Socket**: Base TCP client socket for connecting to servers
- **ServerSocket**: TCP server socket for accepting incoming connections
- **UdpSocket**: UDP socket for connectionless communication
- **Network Utilities**: Helper functions for common networking tasks

## Installation

### Using CMake (Recommended)

1. Clone the repository:

```shell script
git clone <repository-url>
cd sussy_socket
```

2. Build the library:

```shell script
mkdir build
cd build
cmake ..
make
```

### Adding to Existing CMake Project

Add sussy_socket as a subdirectory to your CMake project:

```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/sussy_socket)

# Link against your target
target_link_libraries(your_target PRIVATE sussy_socket)
```

### Manual Integration

Copy the `include/sussy_socket/` directory and `src/` files to your project:

```c++
#include "sussy_socket/Socket.h"
#include "sussy_socket/ServerSocket.h"
#include "sussy_socket/UdpSocket.h"
```

## Requirements

- **C++ Standard**: C++23 or later
- **Build System**: CMake 3.28 or later
- **Platform**: POSIX-compliant systems (Linux, macOS, Unix)

## Running Examples

The library includes several example applications demonstrating different use cases:

### Build Examples

Examples are built automatically when using the standalone build:

```shell script
mkdir build
cd build
cmake ..
make
```

This creates the following executables:

- `socket_client_example` - TCP client example
- `socket_server_example` - TCP server example
- `udp_listener_example` - UDP receiver example
- `udp_talker_example` - UDP sender example

### TCP Examples

**Run TCP Server:**

```shell script
./socket_server_example
```

**Run TCP Client (in another terminal):**

```shell script
./socket_client_example
```

### UDP Examples

**Run UDP Listener:**

```shell script
./udp_listener_example
```

**Run UDP Talker (in another terminal):**

```shell script
./udp_talker_example
```

## Quick Usage Example

### TCP Client

```c++
#include "sussy_socket/Socket.h"

int main() {
    Socket client;
    client.connect("127.0.0.1", "8080");
    
    std::string message = "Hello Server!";
    std::vector<uint8_t> data(message.begin(), message.end());
    client.send(data);
    
    return 0;
}
```

### UDP Communication

```c++
#include "sussy_socket/UdpSocket.h"

int main() {
    UdpSocket udp;
    
    std::string message = "Hello UDP!";
    std::vector<uint8_t> data(message.begin(), message.end());
    udp.send_to(data, "127.0.0.1", "4950");
    
    return 0;
}
```

## Build Options

- `BUILD_EXAMPLES`: Enable/disable building example applications (default: ON)

```shell script
cmake -DBUILD_EXAMPLES=OFF ..
```

## License

## Acknowledgments

Built with inspiration from Beej's Guide to Network Programming, providing a solid foundation for network socket
programming concepts and best practices.