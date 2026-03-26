# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`sussy_socket` is a C++23 static library providing RAII-based wrappers around POSIX TCP and UDP sockets. Everything
lives in the `sussy_socket` namespace.

## Build

```bash
mkdir build && cd build
cmake ..
make
```

To build without examples:

```bash
cmake -DBUILD_EXAMPLES=OFF ..
make
```

There is no test suite. Validate changes by running the example applications manually (see `examples/`).

## Code Style

Enforced via `.clang-format` and `.clang-tidy`:

- 2-space indentation, 80-column limit, LLVM style
- Naming: `CamelCase` for classes, `lower_case` for functions/variables/parameters/namespaces
- Run `clang-format -i <file>` and `clang-tidy <file>` before committing

## Architecture

Three main classes, all non-copyable (copy deleted) with move semantics enabled and RAII destructors:

- **`TcpSUSSY_SOCKET`** — TCP client. Connects to a server on construction. `send_data()` / `receive_data()` use a
  4-byte length-prefix protocol (network byte order). Also used by `TcpAcceptor` to represent accepted connections (
  `TcpSocket` is a friend of `TcpAcceptor`).
- **`TcpAcceptor`** — TCP server. Binds and listens on construction. `accept_connection()` returns a `TcpSocket`.
  Configures `SO_REUSEADDR` and a `SIGCHLD` handler for child process reaping.
- **`UdpSocket`** — UDP. Default constructor creates a talker (no bind); port constructor creates a listener (binds).
  `send_to()` targets a specific IP/port; `receive_from()` blocks and returns sender info. Buffer size is 2048 bytes.
- **`netutils`** (namespace `sussy_socket::net`) — `sigchld_handler` and `get_in_addr` (IPv4/IPv6 agnostic address
  extraction).

**TCP wire format:** `[uint32_t size (big-endian)][payload bytes]` — both `send_data` and `receive_data` loop until all
bytes are transferred.

**UDP:** raw datagrams, no framing.

Headers are under `include/sussy_socket/`, implementations under `src/`. Examples are in `examples/` and built as
separate executables (`socket_client_example`, `socket_server_example`, `udp_listener_example`, `udp_talker_example`).