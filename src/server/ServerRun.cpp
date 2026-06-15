#include "Logging.hpp"
#include "Server.hpp"

#include "Signals.hpp"

#include <cerrno>
#include <csignal>
#include <cstdlib> // for exit
#include <cstring>
#include <unistd.h> // for close

/**
 *	\brief pollLoop() introduces the while(1) networking loop that
 * 	will run for the duration of the webserver.
 *
 *	- calls poll() with the array of pollfd structs in the
 *    Server's poll vector. -1 as third argument means no timeout.
 * 	- calls process() to process the results
 *
 *	If poll fails (an unlikely system call failure), webserver exits.
 */

int Server::pollLoop(void) {

  logging::log(logging::Debug, "<pollLoop>");

  while (1) {
    if (checkSignal() == SIGINT)
      return 0;
    const int res = poll(
        _fds.data(), (nfds_t)_fds.size(),
        TIMEOUT * 1000); // without restriction to _fds.size this cast is unsafe
    // logging::log(logging::Debug, "poll()");
    if (checkSignal() == SIGINT)
      return 0;
    if (res == -1) {
      std::ostringstream msg;
      msg << "poll: " << std::strerror(errno);
      logging::log(logging::Error, msg.str());
      return 1;
    }
    process();
  }
}

void Server::serveAll(void) {
  for (std::map<int, Connection>::iterator it = _clientMap.begin();
       it != _clientMap.end(); it++) {
    (it->second).serve();
    (it->second).resetConditions();
  }
}

/**
*
*	\brief	process() iterates through Server's vector of poll_fds
*	after poll() has been called. For each:
*
*	- Checks each fd's revents field, which was set by poll()
*	- Calls the relevant handling function, which will either
      - set conditions in a client connection, or accept new connections
*	- if Connection is markeid for deletion, close and delete
*	- reset _conditions
*
*	After iteration is complete
*	- calls function to serve all connections
*	- adds any newly accepted Connections to _fds and _clientMap
*	- clears list of new Connections to be added
*
*/

void Server::process(void) {

  // sleep(1); // can be used to slow down loop for debugging
  // logging::log(logging::Debug, "Process");
  time_t timeNow = time(NULL);
  for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end();) {

    int type = getSocketType(it->fd);
    handleCondition(*it, type,
                    timeNow); // sets conditions in client Connection, or
                              // accepts new connections
    // std::cout << getFdInfoString(*it, it->fd, type);
    if (type == IS_CLIENT) {
      if (newCGISocketAdded(it->fd) != true) {
        if (type == IS_CLIENT &&
            timeNow - _clientMap.at(it->fd).getTimeLastActive() >= TIMEOUT) {
          logging::log2(logging::Debug, it->fd,
                        " scheduleForDemolition() in Server::process()");
          _clientMap.at(it->fd).scheduleForDemolition();
        }
      }
    }
    if (type != IS_LISTENER && shouldBeDeleted(it->fd, type) == true) {
      _deleteFdBatch.insert(std::make_pair(it->fd, type));
    }
    it->revents = 0;
    it++;
  }
  closeAndDeleteBatch();
  serveAll();
  _fds.insert(_fds.end(), _newFdBatch.begin(), _newFdBatch.end());
  updateEvents();
  _newFdBatch.clear();
}

bool Server::newCGISocketAdded(int Fd) {
  Connection *connection = &_clientMap.at(Fd);
  // int potentialNewSocket = clientMap.at(Fd)->_socketForward;
  if (connection->getCgiFinishedStatus() == true) {
    return false;
  }
  int fwdSock = connection->getSockForward();
  if (fwdSock != -1 && _fwdMap.find(fwdSock) == _fwdMap.end()) {
    logging::log2(logging::Debug, "Got a new forward socket: ", fwdSock);
    _fwdMap.insert(std::make_pair(fwdSock, connection));
    const pollfd newFd = {
        fwdSock, determineEventsFwd(connection->getConditionsWanted()), 0};
    _newFdBatch.push_back(newFd);
    return true;
  }
  return false;
}

void Server::updateEvents(void) {
  for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end();) {
    int type = getSocketType(it->fd);
    int conditionsWanted;
    if (type == IS_LISTENER) {
      it++;
      continue;
    }
    if (type == IS_CLIENT) {
      conditionsWanted = _clientMap.at(it->fd).getConditionsWanted();
      it->events = determineEventsClient(conditionsWanted);
    }
    if (type == IS_FWD) {
      Connection *clientConnection = _fwdMap.at(it->fd);
      conditionsWanted = clientConnection->getConditionsWanted();
      it->events = determineEventsFwd(conditionsWanted);
    }
    it++;
  }
}

short Server::determineEventsClient(int ConditionsWanted) {

  short events = POLLERR | POLLHUP;
  if (ConditionsWanted & SockWrite) {
    events = events | POLLOUT | POLLRDHUP;
    // TODO rdhup good here?
  }
  if (ConditionsWanted & SockRead) {
    events = events | POLLIN | POLLRDHUP;
    // TODO rdhup good here?
  }
  return (events);
}

short Server::determineEventsFwd(int ConditionsWanted) {

  short events = POLLERR | POLLHUP;
  if (ConditionsWanted & FSockWrite) {
    events = events | POLLOUT | POLLRDHUP;
    // TODO rdhup good here?
  }
  if (ConditionsWanted & FSockRead) {
    events = events | POLLIN | POLLRDHUP;
    // TODO rdhup good here?
  }
  return (events);
}

/* shouldBeDeleted is called only by Server::process, which
  guarantees that Type will be IS_CLINET or IS_FWD */

bool Server::shouldBeDeleted(int Fd, int Type) {
  if (Type == IS_CLIENT) {
    return (_clientMap.at(Fd).getDeleteStatus());
  } else if (Type == IS_FWD) {
    int clientFd = _fwdMap.at(Fd)->getSock();
    // There are 3 instances where a forward socket should be deleted:
    // 1. if the client has already been deleted (not currently possible)
    if (_clientMap.find(clientFd) == _clientMap.end())
      return true;
    // 2. if the forward socket itself is marked for deletion (CGI)
    if (_clientMap.at(clientFd).getCgiFinishedStatus() == true)
      return true;
    // 3. if the forward socket's client is marked for deletion
    return (_clientMap.at(clientFd).getDeleteStatus());
  } else {
    logging::log(logging::Error, "Server::shouldBeDeleted "
                                 "expected IS_CLIENT or IS_FWD fd");
    exit(1);
  }
}

void Server::closeAndDeleteBatch(void) {
  for (std::map<int, int>::iterator it = _deleteFdBatch.begin();
       it != _deleteFdBatch.end(); it++) {
    logging::log3(logging::Debug, it->first,
                  " will be closed and deleted from map of type ",
                  getTypeString(it->second));
    if (it->second == IS_FWD) {
      _fwdMap.erase(it->first);
    } else if (it->second == IS_CLIENT) {
      _clientMap.erase(it->first);
    } else {
      logging::log(logging::Error, "Server::closeAndDeleteBatch "
                                   "expected IS_CLIENT or IS_FWD");
      exit(1);
    }
    close(it->first);
  }
  for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end();) {
    if (_deleteFdBatch.find(it->fd) != _deleteFdBatch.end()) {
      logging::log2(logging::Debug, it->fd,
                    " will be removed from pollfd vector");
      it = _fds.erase(it);
    } else {
      it++;
    }
  }
  _deleteFdBatch.clear();
}

// adds new connection to _clientMap -- move to pollhandling??

void Server::addConnectionToMap(int ListenerFd,
                                const struct ClientAddr &Candidate) {

  const Website *website;
  try {
    website = _listenMap.at(ListenerFd);
  } catch (std::out_of_range &e) {
    logging::log(
        logging::Error,
        "addConnectionToMap(): Connection could not be created because "
        "ListenerFd was not found in _listenMap. This should never happen.");
  }
  Connection newConnection(Candidate.clientSock, Candidate.addr,
                           Candidate.addrSize, *website);
  _clientMap.insert(std::make_pair(Candidate.clientSock, newConnection));
}
