// TODO add 42 header

#include "Networking.hpp"
#include "NetworkingDefines.hpp"


// clear_socket() is a wrapper for setsockopt()
// This function is meant to clear a socket, just in case the same socket
// was in use minutes ago, and hasn't yet been systematically cleared by the OS.
// 
// NOTE: setsockopt() is still largely a black box. I need read to more about the
// arguments. I also have yet to encounter a situation where it's clearly
// necessary to clear a socket, but apparently it happens.
// I am only including this step because Beej's Guide deems it necessary /
// useful. TODO read more.

int networking::clear_socket(const int sock) {
  int yes = 1;
  int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (ret == -1) {
    std::ostringstream msg;
    msg << "setsockopt: " << std::strerror(errno);
    logging::log(logging::Error, msg.str());
  }
  return (ret);
}

// bind_to_ip() is a wrapper for bind().
// This function assigns the listening socket to server's ip
// using bind() and logs an error if bind() fails.
//
// NOTE: Assumes that addrinfo is initialized, but if it wasn't,
// bind() would fail anyway.
//
// RETURN: the result of bind(): 0 on success, -1 on failure

int networking::bind_to_ip(const int sock, const struct addrinfo *p) {

  int ret = bind(sock, p->ai_addr, p->ai_addrlen);
  if (ret != 0) {
    close(sock);
    std::string msg(gai_strerror(ret));
    logging::log(logging::Info,
                 "bind: " + msg + " will continue trying sockets");
  }
  return (ret);
}

// create_socket() is a wrapper for library function socket()
// This function logs an error if socket() fails.
//
// TODO: Currently, we using the contents of addrinfo to set protocol.
// The contents should always specfiy TCP thanks to our "hints" input
// (see Networking_server_init.cpp). If we wanted to, we could also
// hardcode TCP here. What's cleaner?

int networking::create_socket(const struct addrinfo *p) {

  int sock = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol); // could set ai_protocol manually to TCP?
  if (sock == -1) {
    std::ostringstream msg;
    msg << "socket: " << std::strerror(errno)
        << " (will continue trying sockets)";
    logging::log(logging::Info, msg.str());
  }
  return (sock);
}

// get_server_socket() creates a socket for the server
// to begin listening.
//
// NOTE: Assumes that argument server_info has already been initialized.
// The function iterates through the server's linked list of addrinfo structs,
// until it is either able to create a socket or reaches the end.
//
// If no socket it found, an exception will be thrown. Server_info
// will first be freed, because this exception throws us back to main().
//
// If a socket IS found, it will be cleared (just in case the same socket
// was in use minutes ago, and hasn't yet been systematically cleared by the OS
// ... TODO read a bit more about this.) The socket will then be bound to
// the server's IP address. In the case that bind() fails, the function
// will continue trying to create, clear, and bind a socket with any
// remaining addrinfos.
//
// RETURNS: file descriptor for available socket
//
int networking::get_server_socket(struct addrinfo *server_info) {

  int sock;
  const struct addrinfo *p;
  for (p = server_info; p != NULL; p = p->ai_next) {
    sock = create_socket(p);
    if (sock == -1) {
      continue;
    }
    clear_socket(sock);
    if (bind_to_ip(sock, p) == -1) {
      continue;
    }
    break;
  }
  if (p == NULL) {
    freeaddrinfo(server_info);
    throw std::runtime_error("server failed to find a socket");
  } else {
    logging::log(logging::Debug, "sever found socket. bind: success");
  }
  return (sock);
}

void networking::set_to_listen(const int sock) {

  if (listen(sock, BACKLOG == -1)) {
    std::ostringstream msg;
    msg << "listen: " << std::strerror(errno);
    throw std::runtime_error(msg.str());
  }
  logging::log(logging::Debug, "server is listening for connections...");
}

//////////////////////////////////////////////////////////////////////////////
