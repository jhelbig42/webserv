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

void Server::setSockRead(int Fd) {
  const std::map<int, Connection>::iterator itC = _clientMap.find(Fd);
  if (itC != _clientMap.end()) {
    (itC->second).addToConditions(SockRead);
  } else {
    logging::log3(logging::Error, "setSockRead(): ", Fd,
                  " not found in _clientMap. (This should never happen)");
  }
}

void Server::setFSockRead(int Fd) {
  const std::map<int, Connection &>::iterator itC = _fwdMap.find(Fd);
  if (itC != _fwdMap.end()) {
    (itC->second).addToConditions(FSockRead);
  } else {
    logging::log3(logging::Error, "setFSockRead(): ", Fd,
                  " not found in _fwdMap. (This should never happen)");
  }
}

void Server::handleNewConnection(int Fd) {
  ClientAddr candidate;
  if (acceptConnection(Fd, &candidate) != -1) {
    addConnectionToMap(Fd, candidate);
    const short events =
        POLLIN | POLLERR | POLLHUP | POLLPRI | POLLRDHUP | POLLOUT;
    const pollfd newFd = {candidate.clientSock, events, 0};
    _newFdBatch.push_back(newFd);
  }
}

// \brief	handlePollin() handles POLLIN:
//
// \param	fd of socket for which poll() returned POLLIN

void Server::handlePollin(int Fd) {
  logging::log2(logging::Debug, "POLLIN: fd ", Fd);
  if (socketIsListener(Fd)) {
    handleNewConnection(Fd);
    return;
  }
  if (socketIsClient(Fd)) {
    setSockRead(Fd);
    return;
  }
  if (socketIsFwd(Fd)) {
    setFSockRead(Fd);
    return;
  }
}
