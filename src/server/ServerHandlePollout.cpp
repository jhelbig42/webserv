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

// handlePollout() handles POLLOUT:
// 		Writing is now possible.

void Server::handlePollout(int Fd) {
 // logging::log2(logging::Debug, "POLLOUT: fd ", Fd);
  const std::map<int, Connection>::iterator itC = _clientMap.find(Fd);
  if (itC != _clientMap.end()) {
    // logging::log2(logging::Debug, "Ready to send to ", Fd);
    (itC->second).addToConditions(SockWrite);
  } else {
    logging::log(logging::Error, "process: Connection not found in map "
                                 "container (This should never happen)");
  }
}
