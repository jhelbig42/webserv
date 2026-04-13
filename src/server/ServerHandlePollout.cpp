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

void Server::setSockWrite(int Fd){
  const std::map<int, Connection>::iterator itC = _clientMap.find(Fd);
  if (itC != _clientMap.end()) {
    // logging::log2(logging::Debug, "Ready to send to ", Fd);
    (itC->second).addToConditions(SockWrite);
  } else {
    logging::log3(logging::Error, "setSockWrite(): ", Fd, " not found in _clientMap. (This should never happen)");
  }
}

void Server::setFSockWrite(int Fd){
    const std::map<int, Connection&>::iterator itC = _fwdMap.find(Fd);
    if (itC != _fwdMap.end()) {
      (itC->second).addToConditions(FSockWrite);
    } else {
      logging::log3(logging::Error, "setFSockWrite(): ", Fd, " not found in _fwdMap. (This should never happen)");
    }
}

// handlePollout() handles POLLOUT:
// 		Writing is now possible.

void Server::handlePollout(int Fd) {
 // logging::log2(logging::Debug, "POLLOUT: fd ", Fd);
	if (socketIsClient(Fd)) {
		setSockWrite(Fd);
		return;
	}
	if (socketIsFwd(Fd)) {
		setFSockWrite(Fd);
		return;
	}
}
