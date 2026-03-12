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

Server::Server(const std::list<Website> &Websites) {
  
  memset(&_hints, 0, sizeof _hints); // init struct to empty;
  _hints.ai_family = AF_UNSPEC;     // allows either IPv4 or IPv6
  _hints.ai_socktype = SOCK_STREAM; // for TCP stream sockets
  _hints.ai_flags = AI_PASSIVE;     // autofill my IP
  
  initNetworking(Websites);
}

void Server::initNetworking(const std::list<Website> &Websites) {

  // iterate through all websites from config file
  for (std::list<Website>::const_iterator itW = Websites.begin();
       itW != Websites.end(); itW++) {
    const std::list<Listen> interfaces = itW->getInterfaces();

    // iterate through all interfaces of a given website
    for (std::list<Listen>::const_iterator itI = interfaces.begin();
         itI != interfaces.end(); itI++) {
      try {
	  	const Socket sock = initListeningSocket(*itI, *itW);
      	sockets.push_back(sock);
	  }
	  catch (const std::exception &e) {
	  	logging::log(logging::Error, e.what());
	  }
    }
  }
}

Socket Server::initListeningSocket(const Listen &Interface,
                                   const Website &Web) {

  struct addrinfo *serverInfo = getInfo(Interface);

  (void)serverInfo;
  freeaddrinfo(serverInfo);
  // set up socket
  Socket socket;
  return (socket);
}

struct addrinfo *Server::getInfo(const Listen &Interface) {

  struct addrinfo *info;
  unsigned short port;

  (void)port;
  const int ret = getaddrinfo(NULL, PORT, &_hints, &info); // NULL = localhost
  if (ret != 0) {
    const std::string msg(gai_strerror(ret));
    throw std::runtime_error("getaddrinfo: " + msg);
  }
  // logging::log(logging::Debug, "addrinfo server_info created");
  // logging::log(logging::Debug, addrinfoToStr(info, "server_info:"));
  return (info);
}

// get_addr_info_str() is only for logging purposes.
// The function builds a string from the contents of an addrinfo struct
// and an additional msg, which I've used to add a label for this struct.
//
// NOTE: This function should not be used with uninitialized addrinfo structs,
// in which the values might be garbage. Currently, it is only used by
// getServerInfo(), which assures initialization.
//
// Currently static due to one-time use. In the future, may be useful for
// logging client addrinfo, in which case it would need to be added back
// to the header, and could be moved to a different (debug / helper?) .cpp
//
// RETURNS: a string with all info to be printed

std::string Server::addrinfoToStr(const struct addrinfo *Info,
                                  const std::string &Msg) {
  std::ostringstream oss;
  oss << "\n\t" << Msg << "\n"
      << "\tai_flags = " << Info->ai_flags << "\n"
      << "\tai_family = " << Info->ai_family << "\n"
      << "\tai_socktype = " << Info->ai_socktype << "\n"
      << "\tai_protocol = " << Info->ai_protocol << "\n"
      << "\tai_addrlen = " << Info->ai_addrlen << "\n";
  return (oss.str());
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
