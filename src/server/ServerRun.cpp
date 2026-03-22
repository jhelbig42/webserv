#include "Server.hpp"
#include "Logging.hpp"

#include <cerrno>
#include <cstring>
#include <unistd.h> // for close


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

bool Server::reventsAreTerminal(int revents){
	if ((revents & POLLNVAL) | (revents & POLLERR) |
        (revents & POLLHUP) | (revents & POLLPRI) |
        (revents & POLLRDHUP)) {
			return true;
	}
	return false;
}

void Server::process(void) {

	logging::log(logging::Debug, "Process");
	for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end();) {
    	if (reventsAreTerminal(it->revents)){
      		handleTerminalCondition(*it);
    	} else {
      		handleServableCondition(*it);
    	}
		if (clientMap.find(it->fd) != clientMap.end()){ // if socket belongs to a client
		// TODO make this a getter socketIsClient
			if (clientMap.at(it->fd).getDeleteStatus() == true) { // if should be deleted, delete
      			close(it->fd);
      			clientMap.erase(it->fd);
      			it = fds.erase(it);
    		}	
			else { // else, hand off Connection object to be further handled
        		clientMap.at(it->fd).processData();
        		clientMap.at(it->fd).resetConditions();
      			it++;
    		}
		}
  	}
  	fds.insert(fds.end(), newFdBatch.begin(), newFdBatch.end());
  	newFdBatch.clear();
}

// acceptConnections() is a a wrapper for accept(), which extracts
// the first connection request on the queue of pending connections
// for the listening socket. Creates new socket for the
// incoming connection.
//
// RETURNS: fd for new socket

int Server::acceptConnection(int ListenerFd, ClientAddr *Candidate) {

  Candidate->clientSock = accept(
      ListenerFd, (struct sockaddr *)&Candidate->addr, &Candidate->addrSize);
  if (Candidate->clientSock == -1) {
    std::ostringstream msg;
    msg << "accept: " << std::strerror(errno)
        << " (can continue trying to accept connections)";
    logging::log(logging::Error,
                 msg.str()); // may downgrade log level at some point
    return (-1);
  }
  logging::log2(logging::Debug, "Connection accepted on socket ",
                Candidate->clientSock);
  // printFcntlFlags(Candidate->clientSock);
  return (0);
}

void Server::addConnectionToMap(int ListenerFd, const struct ClientAddr &Candidate) {

  const Website *website;
  try {
  	website = listenMap.at(ListenerFd);
  } catch (std::out_of_range &e) {
  	logging::log(logging::Error, "addConnectionToMap(): Connection could not be created because ListenerFd was not found in listenMap. This should never happen.");
  }
  Connection newConnection(Candidate.clientSock, Candidate.addr, Candidate.addrSize, *website);
  clientMap.insert(std::make_pair(Candidate.clientSock, newConnection));
}
