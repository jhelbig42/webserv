/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking_run.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 16:36:50 by hallison          #+#    #+#             */
/*   Updated: 2026/02/27 12:22:18 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

// TODO These functions could be made static:
// process(), acceptConnection(), addConnectionToMap()
// But then they would lose sight of networking::ClientAddr struct
// and netoworking::read_data().
// Decide on most elegant arrangement.

void networking::pollLoop(const int Sock);
void networking::process(const int ListenSock, std::map<int, Connection> &CMap,
                         std::vector<pollfd> &Fds);
int networking::acceptConnection(const int ListenSock, ClientAddr *Candidate);
void networking::addConnectionToMap(const struct ClientAddr &Candidate,
                                    std::map<int, Connection> &CMap);

// pollLoop() introduces the while(1) networking loop that will run
// for the duration of the webserver. This function:
//
// • Declares cMap, a map of clients (key = fd, value = Connection object)
// • Declares fds, a vector of pollfd structs
//  - Pollfd structs are library-defined and required by poll()
//  - Placing pollfds in a vector is useful for constant re-sizing,
//    as well as keeping constant track of the collection size.
//    Can ALSO be used as a normal array, which poll() expects.
// • Adds the server's listening socket as the first member of pollfd vector
// • Enters the polling loop, which
// 	- continuously checks and marks which pollfds are ready for I/O
// 	- processes the ones that are marked

void networking::pollLoop(const int Sock) {

  std::map<int, Connection> cMap;
  std::vector<pollfd> fds;

  const pollfd listener = {Sock, POLLIN, 0};
  fds.push_back(listener);

  while (1) {
    const int res =
        poll(fds.data(), (nfds_t)fds.size(),
             -1); // without restriction to fds.size this cast is unsafe
    //logging::log(logging::Debug, "poll()");
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

void networking::process(const int ListenSock, std::map<int, Connection> &CMap,
                         std::vector<pollfd> &Fds) {

  //logging::log(logging::Debug, "Networking::Process()\n");
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
        // cMap.at(it->fd).serve(MAX_REQUEST);
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
