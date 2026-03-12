#include "Server.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "Networking.hpp"
// #include "NetworkingDefines.hpp" // Can be removed?
#include <cerrno>  // for errno
#include <cstdlib> // exit()
#include <cstring> // for strerror
#include <exception>
#include <fcntl.h> // TODO delete before submission, only for debugging
#include <map>
#include <ostream>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>


Socket initListeningSocket(const Listen &Interface, const Website &Web){
	
	//struct addrinfo *serverInfo = getServerInfo(); // bring in from networking
	Socket socket;
	return(socket);
}

Server::Server(const std::list<Website> &Websites) {
  for (std::list<Website>::const_iterator itW = Websites.begin();
       itW != Websites.end(); itW++) {
    logging::log(logging::Debug, "Setting up website...");

    const std::list<Listen> interfaces = itW->getInterfaces();
    for (std::list<Listen>::const_iterator itI = interfaces.begin();
         itI != interfaces.end(); itI++){
			const Socket sock = initListeningSocket(*itI, *itW);
			sockets.push_back(sock);
		}
  }
}

void Server::pollLoop(void) {

  logging::log(logging::Debug, "<pollLoop>");
  /*
    const pollfd listener = {Sock, POLLIN, 0};
    fds.push_back(listener);

    while (1) {
      const int res =
          poll(fds.data(), (nfds_t)fds.size(),
               -1); // without restriction to fds.size this cast is unsafe
      // logging::log(logging::Debug, "poll()");
      if (res == -1) {
        std::ostringstream msg;
        msg << "poll: " << std::strerror(errno);
        logging::log(logging::Error, msg.str());

        // TODO How to best handle poll() failure?
        // Currently, logging error and exiting
        // Could also log Warning and continue.
      }
      process(Sock, cMap, fds);
    }
    */
}

// process() iterates through the vector of fds and handles the flags
// that were set by poll() in fd[i]->events & fd[i]->revents.
//
// If one of the handling functions sets the Connection's _delete field
// to true, due to client hang-up or error, the fd is closed,
// and both the Connection & the fd are deleted.
//
// If a new connection is discovered, a new Connection and fd
// are added.
//
// TODO Handle additional flags

/*
void networking::process(const int ListenSock, std::map<int, Connection> &CMap,
                         std::vector<pollfd> &Fds) {

  // logging::log(logging::Debug, "Networking::Process()\n");
  std::vector<pollfd> newFdBatch;
  for (std::vector<pollfd>::iterator it = Fds.begin(); it != Fds.end();) {
    if ((it->revents & POLLNVAL) | (it->revents & POLLERR) |
        (it->revents & POLLHUP) | (it->revents & POLLPRI) |
        (it->revents & POLLRDHUP)) {
      handleTerminalCondition(it->revents, it->fd, CMap);
    } else {
      handleServableCondition(ListenSock, it->revents, it->fd, CMap,
                              newFdBatch);
    }
    if (it->fd != ListenSock && CMap.at(it->fd).getDeleteStatus() == true) {
      close(it->fd);
      CMap.erase(it->fd);
      it = Fds.erase(it);
    } else {
      if (it->fd != ListenSock) {
        CMap.at(it->fd).processData();
        CMap.at(it->fd).resetConditions();
      }
      it++;
    }
  }
  Fds.insert(Fds.end(), newFdBatch.begin(), newFdBatch.end());
  newFdBatch.clear();
}

// acceptConnections() is a a wrapper for accept(), which extracts
// the first connection request on the queue of pending connections
// for the listening socket. Creates new socket for the
// incoming connection.
//
// RETURNS: fd for new socket

int networking::acceptConnection(const int ListenSock, ClientAddr *Candidate) {

  Candidate->clientSock = accept(
      ListenSock, (struct sockaddr *)&Candidate->addr, &Candidate->addrSize);
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
  printFcntlFlags(Candidate->clientSock);
  return (0);
}

// printFcntlFlags() is a temporary debug function that makes use
// of forbidden function fcntl(). This function is used to check
// if a particular fd is blocking, and print the results.

void networking::printFcntlFlags(const int Sock) {
  const int flags = fcntl(Sock, F_GETFL);
  logging::log3(logging::Debug, Sock, " fcntl flags = ", flags);
  if (flags & O_NONBLOCK) {
    logging::log2(logging::Debug, Sock, " is NON-BLOCKING");
  } else {
    logging::log2(logging::Debug, Sock, " is BLOCKING");
  }
}

void networking::addConnectionToMap(const struct ClientAddr &Candidate,
                                    std::map<int, Connection> &CMap) {

  const Connection newConnection =
      Connection(Candidate.clientSock, Candidate.addr, Candidate.addrSize);
  CMap.insert(std::make_pair(Candidate.clientSock, newConnection));
}
*/
