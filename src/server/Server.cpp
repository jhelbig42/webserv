#include "Server.hpp"
#include <cstring>  // for memset()
#include <unistd.h> // for close()

Server::Server(const std::list<Website> &Websites) {

  memset(&_hints, 0, sizeof _hints); // init struct to empty;
  _hints.ai_family = AF_UNSPEC;      // allows either IPv4 or IPv6
  _hints.ai_socktype = SOCK_STREAM;  // for TCP stream sockets
  _hints.ai_flags = AI_PASSIVE;      // autofill my IP

  initNetworking(Websites);
}

bool Server::socketIsListener(int Fd) {
  if (listenMap.find(Fd) != listenMap.end()) {
    return true;
  }
  return false;
}

bool Server::socketIsClient(int Fd) {
  if (clientMap.find(Fd) != clientMap.end()) {
    return true;
  }
  return false;
}


Server::~Server(void) {
  for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++) {
    close(it->fd);
  }
}
