#include "Server.hpp"
#include "Logging.hpp"

#include <cerrno>
#include <cstring>


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

void Server::process(void) {

	logging::log(logging::Debug, "Process");
  /*
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
*/
}

/*
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

