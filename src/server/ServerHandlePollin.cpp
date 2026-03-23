#include "Connection.hpp"
#include "Logging.hpp"
#include "Server.hpp"
#include <map>
#include <poll.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

// handlePollin() handles POLLIN:
// 		There is data to read.
//	TODO break into helper functions for readability

void Server::handlePollin(int Fd){ 
  logging::log2(logging::Debug, "POLLIN: fd ", Fd);
  if (listenMap.find(Fd) !=
      listenMap.end()) { // listening socket got new connection
                         // TODO make this a getter if connectionIsListener
    ClientAddr candidate;
    if (acceptConnection(Fd, &candidate) != -1) {
      addConnectionToMap(Fd, candidate);
      const short events =
          POLLIN | POLLERR | POLLHUP | POLLPRI | POLLRDHUP | POLLOUT;
      const pollfd newFd = {candidate.clientSock, events, 0};
      newFdBatch.push_back(newFd);
    }
  } else {
    const std::map<int, Connection>::iterator itC = clientMap.find(Fd);
    if (itC != clientMap.end()) {
      (itC->second).addToConditions(SockRead);
    } else {
      logging::log(logging::Error, "process: Connection not found in map "
                                   "container (This should never happen)");
      // could be removed after thorough testing
    }
  }
}

