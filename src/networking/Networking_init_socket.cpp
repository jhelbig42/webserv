#include "Networking.hpp"
#include "NetworkingDefines.hpp"

// Networking init: Functions related to initial server setup,
// Expected to be used only on program start
// TODO change several functions to static

//////////////////////////////////////////////////////////////////////////////

int networking::clear_socket(int sock) {
  int yes = 1;
  int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (ret == -1) {
    std::ostringstream msg;
    msg << "setsockopt: " << std::strerror(errno);
    logging::log(logging::Error, msg.str());
  }
  return (ret);
}

int networking::bind_to_ip(int sock, struct addrinfo *p) {

  int ret = bind(sock, p->ai_addr, p->ai_addrlen);
  if (ret != 0) {
    close(sock);
    std::string msg(gai_strerror(ret));
    logging::log(logging::Info,
                 "bind: " + msg + " will continue trying sockets");
  }
  return (ret);
}

int networking::create_socket(struct addrinfo *server_info,
                              struct addrinfo *p) {

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

// starts the process of obtaining server (listening) socket
int networking::get_server_socket(struct addrinfo *server_info) {

  int sock;
  struct addrinfo *p;
  for (p = server_info; p != NULL; p = p->ai_next) {
    sock = create_socket(server_info, p);
    if (sock == -1) {
      continue;
    }

    clear_socket(sock); // not necessary until we have multiple connections?

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

void networking::set_to_listen(int sock) {

  if (listen(sock, BACKLOG == -1)) {
    std::ostringstream msg;
    msg << "listen: " << std::strerror(errno);
    throw std::runtime_error(msg.str());
  }
  logging::log(logging::Debug, "server is listening for connections...");
}

//////////////////////////////////////////////////////////////////////////////
