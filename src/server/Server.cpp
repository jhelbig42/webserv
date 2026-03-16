#include "Server.hpp"
#include <unistd.h> // for close()
#include <cstring>  // for memset()

Server::Server(const std::list<Website> &Websites) {
  
  memset(&_hints, 0, sizeof _hints); // init struct to empty;
  _hints.ai_family = AF_UNSPEC;     // allows either IPv4 or IPv6
  _hints.ai_socktype = SOCK_STREAM; // for TCP stream sockets
  _hints.ai_flags = AI_PASSIVE;     // autofill my IP
  
  initNetworking(Websites);
}

Server::~Server(void) {
 // for (std::vector<Socket>::iterator it = sockets.begin(); it != sockets.end();
	  // it++) {
	//close(it->fd);
}

//