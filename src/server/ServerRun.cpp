#include "Logging.hpp"
#include "Server.hpp"

#include <cerrno>
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

void	Server::serveAll(void){
	for (std::map<int, Connection>::iterator it = _clientMap.begin(); it != _clientMap.end(); it++) {
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
    	handleCondition(*it); // sets conditions in client Connection, or accepts new connections
		if (shouldBeDeleted(it->fd) == true) {
			closeAndDelete(it->fd);
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

void Server::closeAndDelete(int Fd){
       
	close(Fd);
	if (socketIsClient(Fd))
   		_clientMap.erase(Fd);
	if (socketIsFwd(Fd))
		_fwdMap.erase(Fd);
}


bool Server::shouldBeDeleted(int Fd){
	if (socketIsClient(Fd) && _clientMap.at(Fd).getDeleteStatus() == true) {
		return (true);
	}
	int clientFd;
  	try {
    	clientFd = _fwdMap.at(Fd).getSock();
  	}
	catch (std::out_of_range &e) {
    logging::log3(
        logging::Error,
        "shouldBeDeleted(): Fd ", Fd, "not found in _clientMap or _fwdMap"
        "This should never happen.");
		return (false);
  	}
	try {
		if (_clientMap.at(clientFd).getDeleteStatus() == true){
			return (true);
		}
	}
	catch (std::out_of_range &e) {
	logging::log3(logging::Error, "shouldBeDeleted(): Fd ", Fd, " is fwd socket"
				"associated with client socket ");
    logging::log2(
        logging::Error, clientFd, ", not found in _clientMap. Should never happen");
		return (false);
  	}
	return (false);
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
