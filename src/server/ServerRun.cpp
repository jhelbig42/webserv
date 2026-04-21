#include "Logging.hpp"
#include "Server.hpp"

#include <cerrno>
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

void Server::pollLoop(void) {

  logging::log(logging::Debug, "<pollLoop>");

  while (1) {
    const int res =
        poll(_fds.data(), (nfds_t)_fds.size(),
             -1); // without restriction to _fds.size this cast is unsafe
    // logging::log(logging::Debug, "poll()");
    if (res == -1) {
      std::ostringstream msg;
      msg << "poll: " << std::strerror(errno);
      logging::log(logging::Error, msg.str());
      exit(1);
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

  for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end();) {
  	int type = getSocketType(it->fd);
  	if (type != IS_CLIENT && type != IS_FWD) {
    	logging::log3(logging::Error, "handleCondition(): fd ", it->fd,
                  "is not found in _clientMap or _fwdMap.");
    	exit (1);
  	}
    handleCondition(*it, type); // sets conditions in client Connection, or accepts
                          // new connections
    if (shouldBeDeleted(it->fd, type) == true) {
      closeAndDelete(it->fd, type);
      it = _fds.erase(it);
      continue;
    }
    it->revents = 0;
    it++;
  }
  serveAll();
  _fds.insert(_fds.end(), _newFdBatch.begin(), _newFdBatch.end());
  _newFdBatch.clear();
}

void Server::closeAndDelete(int Fd, int type) {

  close(Fd);
  if (socketIsClient(Fd))
    _clientMap.erase(Fd);
  if (socketIsFwd(Fd))
    _fwdMap.erase(Fd);
}

/* shouldBeDeleted is called only by Server::process, which
	guarantees that Type will be IS_CLINET or IS_FWD */

bool Server::shouldBeDeleted(int Fd, int Type) {
  if (Type == IS_CLIENT){
	return (_clientMap.at(Fd).getDeleteStatus());
  }
  else if (Type == IS_FWD) {
  	int clientFd = _fwdMap.at(Fd)->getSock();
  	return (_clientMap.at(clientFd).getDeleteStatus());
  }
  else {
	logging::log(logging::Error, "Server::shouldBeDeleted "
		"expected IS_CLIENT or IS_FWD fd");
		exit (1);
	}
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
