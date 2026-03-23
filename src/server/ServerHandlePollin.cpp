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

// \brief	handlePollin() handles POLLIN:
// 	
// \param	fd of socket for which poll() returned POLLIN

void Server::handlePollin(int Fd){ 
  logging::log2(logging::Debug, "POLLIN: fd ", Fd);
  if (socketIsListener(Fd)) {
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
    }
  }
}

