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
        poll(fds.data(), (nfds_t)fds.size(),
             -1); // without restriction to fds.size this cast is unsafe
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

/**
*
*	\brief	process() iterates through Server's vector of pollfds
*	after poll() has been called. For each:
*
*	- Checks each fd's revents field, which was set by poll()
*	- Calls the relevant handling function, which will either
		- set conditions in a client connection, or accept new connections
*	- If the fd belongs to a client
*		- if Connection is markeid for deletion, close and delete
*		- otherwise, send to parser for next steps
*		- reset _conditions
*
*	After iteration is complete
*	- adds any newly accepted Connections to fds and clientMap
*	- clears list of new Connections to be added
*
*/

void Server::process(void) {

  for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end();) {
    if (reventsAreTerminal(it->revents)) {
      handleTerminalCondition(*it);
    } else {
      handleServableCondition(*it);
    }
    if (clientMap.find(it->fd) !=
        clientMap.end()) { // if socket belongs to a client
                           // TODO make this a getter socketIsClient
      if (clientMap.at(it->fd).getDeleteStatus() ==
          true) { // if should be deleted, delete
        close(it->fd);
        clientMap.erase(it->fd);
        it = fds.erase(it);
        continue;
      } else { // else, hand off Connection object to be further handled
        clientMap.at(it->fd).processData();
        clientMap.at(it->fd).resetConditions();
      }
    }
    it->revents = 0;
    it++;
  }
  fds.insert(fds.end(), newFdBatch.begin(), newFdBatch.end());
  newFdBatch.clear();
}

/** 
*	\brief reventsAreTerminal() checks if poll returned revents
*   that indicate the connection should be terminated, due to client
*   hang-up or error
*
*	\param	revents, the field in pollfd filled by poll()
*	\return true if conditions are found, otherwise false
*/

bool Server::reventsAreTerminal(int revents) {
  if ((revents & POLLNVAL) || (revents & POLLERR) || (revents & POLLHUP) ||
      (revents & POLLPRI) || (revents & POLLRDHUP)) {
    return true;
  }
  return false;
}

// adds new connection to clientMap -- move to pollhandling??

void Server::addConnectionToMap(int ListenerFd,
                                const struct ClientAddr &Candidate) {

  const Website *website;
  try {
    website = listenMap.at(ListenerFd);
  } catch (std::out_of_range &e) {
    logging::log(
        logging::Error,
        "addConnectionToMap(): Connection could not be created because "
        "ListenerFd was not found in listenMap. This should never happen.");
  }
  Connection newConnection(Candidate.clientSock, Candidate.addr,
                           Candidate.addrSize, *website);
  clientMap.insert(std::make_pair(Candidate.clientSock, newConnection));
}
