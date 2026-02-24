/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking_init_socket.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:06:40 by hallison          #+#    #+#             */
/*   Updated: 2026/02/24 15:50:19 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logging.hpp"
#include "Networking.hpp"
#include "NetworkingDefines.hpp"
#include <cerrno>  // for errno
#include <cstddef> // for NULL
#include <cstring> // for strerror
#include <netdb.h>
#include <ostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // for close

static int clearSocket(const int sock);
static int bindToIP(const int sock, const struct addrinfo *p);
static int createSocket(const struct addrinfo *p);
int networking::getServerSocket(struct addrinfo *server_info);
void networking::setToListen(const int sock);

// setToListen() is a wrapper for listen(), which marks the
// server's socket as a "possive socket". This means it will
// be used to accept incoming connection requests using accept().
//
// The second arguments of listen() is backlog, the maximum
// length to which the queue of pending connections may grow.

void networking::setToListen(const int sock) {

  if (listen(sock, BACKLOG == -1)) {
    std::ostringstream msg;
    msg << "listen: " << std::strerror(errno);
    throw std::runtime_error(msg.str());
  }
  logging::log(logging::Debug, "server is listening for connections...");
}

// getServerSocket() creates a socket for the server
// to begin listening.
//
// NOTE: Assumes that server_info has already been initialized.
//
// Iterates through the server's linked list of addrinfo structs,
// until either able to create a socket or reaches the end.
// If no socket found, an exception is thrown. Server_info
// is first be freed, because exception throws back to main().
//
// If socket IS found, it is cleared (just in case the same socket
// was in use minutes ago, and hasn't yet been systematically cleared by the OS
// ... TODO read a bit more about this.) Socket is then assigned to
// server's IP address. If bind() fails, continues trying to create, clear,
// and bind a socket with any remaining addrinfos.
//
// RETURNS: file descriptor for server's listening socket

int networking::getServerSocket(struct addrinfo *server_info) {

  int sock;
  const struct addrinfo *p;
  for (p = server_info; p != NULL; p = p->ai_next) {
    sock = createSocket(p);
    if (sock == -1) {
      continue;
    }
    clearSocket(sock);
    if (bindToIP(sock, p) == -1) {
      continue;
    }
    break;
  }
  if (p == NULL) {
    freeaddrinfo(server_info);
    throw std::runtime_error("server failed to find a socket");
  }
  logging::log(logging::Debug, "sever found socket. bind: success");
  return (sock);
}

// clearSocket() is a wrapper for setsockopt()
// This function is meant to clear a socket, just in case the same socket
// was in use minutes ago, and hasn't yet been systematically cleared by the OS?
//
// NOTE: setsockopt() is still largely a black box. I need read to more about
// the arguments. I also have yet to encounter a situation where it's clearly
// necessary to clear a socket, but apparently it happens.
// I am only including this step because Beej's Guide deems it necessary /
// useful. TODO read more.
//
// RETURN: the result of setsockopt(): 0 on success, -1 on failure

static int clearSocket(const int sock) {
  int yes = 1;
  const int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (ret == -1) {
    std::ostringstream msg;
    msg << "setsockopt: " << std::strerror(errno);
    logging::log(logging::Error, msg.str());
  }
  return (ret);
}

// bindToIP() is a wrapper for bind().
// This function assigns the listening socket to server's ip
// using bind() and logs an error if bind() fails.
//
// NOTE: Assumes that addrinfo is initialized, but if it wasn't,
// bind() would fail anyway.
//
// RETURN: the result of bind(): 0 on success, -1 on failure

static int bindToIP(const int sock, const struct addrinfo *p) {

  const int ret = bind(sock, p->ai_addr, p->ai_addrlen);
  if (ret != 0) {
    close(sock);
    std::string msg(gai_strerror(ret));
    logging::log(logging::Info,
                 "bind: " + msg + " will continue trying sockets");
  }
  return (ret);
}

// createSocket() is a wrapper for library function socket()
// This function logs an error if socket() fails.
//
// TODO: Currently, I'm using the contents of addrinfo to set
// protocol. This should always specfiy TCP thanks to our "hints" input
// (see Networking_server_init.cpp). If we wanted to, we could also
// hardcode TCP here. What's cleaner?

static int createSocket(const struct addrinfo *p) {
  const int sock = socket(p->ai_family, SOCK_STREAM | SOCK_NONBLOCK, 0);
  // 0 = TCP
  if (sock == -1) {
    std::ostringstream msg;
    msg << "socket: " << std::strerror(errno)
        << " (will continue trying sockets)";
    logging::log(logging::Info, msg.str());
  }
  return (sock);
}

//////////////////////////////////////////////////////////////////////////////
