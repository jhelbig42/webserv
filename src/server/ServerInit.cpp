#include "Connection.hpp"
#include "Logging.hpp"
#include "Server.hpp"
#include <cerrno>  // for errno
#include <cstdlib> // exit()
#include <cstring> // for strerror
#include <exception>
#include <map>
#include <ostream>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

/**
 * \brief initNetworking() loops through all Websites in the config,
 * and all ip:port pairs assigned to a given Website.
 *
 * initListeningSocket() is called for each ip:pair, in order
 * to create and set-up listening sockets, and add them to the Server.
 *
 * \param Websites	list of Website objects parsed from config file
 */

void Server::initNetworking(const std::list<Website> &Websites) {

  // iterate through each website from config file
  for (std::list<Website>::const_iterator itW = Websites.begin();
       itW != Websites.end(); itW++) {
    const std::list<Listen> interfaces = itW->getInterfaces();
    std::list<Listen>::const_iterator itI = interfaces.begin();
    if(itI == interfaces.end()) {
      throw std::runtime_error("Bad config file: missing \"listen: <IP:Port>\"");
    }
    // iterate through each IP:port pair of a given website,
    // contained within the Website's Listen struct
    while (itI != interfaces.end()) {
      logging::log2(logging::Debug, "interface = ", itI->ip + ":" + itI->port);
      initListeningSocket(*itI, *itW);
      itI++;
    }
  }
}

/* \brief initListeningSocket() does the following:
 *
 *  1) checks if a given port:pair is valid, i.e. not yet used by us
 *  2) checks if a given port is valid, i.e. could theoretically
 *		be used to create a socket without causing undefined behavior
 *		not caught or indicated by socket() or bind() errno.
 *  3) calls getaddrinfo() via a wrapper, to get an addrinfo struct
 *  4) calls getListeningSocket, kicking off socket creation and set-up
 *  5) frees the addrinfo via freeaddrinfo()
 *  6) adds socket to the Server's vector of poll_fds
 *  7) adds socket to the Server's listenMap
 *
 * \param Pair	ip:port on which we want to listen, needed for socket setup
 * \param Web	reference to associated Website, needed for listenMap
 */

void Server::initListeningSocket(const Listen &Pair, const Website &Web) {
  struct addrinfo *serverInfo;
  int sock;

  checkPair(Pair);      // pair is not already in use
  checkPort(Pair.port); // port is uint16_t

  serverInfo = getAddrInfo(Pair);
  sock = getListeningSocket(serverInfo, Pair);
  freeaddrinfo(serverInfo);

  const pollfd newFd = {sock, POLLIN, 0};
  logging::log2(logging::Debug, "Listening socket created on socket ", sock);
  _fds.push_back(newFd);
  _listenMap.insert(std::make_pair(sock, &Web));
  return;
}

// Checks that IP:port pair is not already being used by another website.
// NGINX allows multiple websites to use the same pair -- It is our
// choice to disallow it.

void Server::checkPair(const Listen &Pair) {

  std::string pair = Pair.ip + ":" + Pair.port;
  logging::log2(logging::Debug, "checkpair():\n", pair);
  if (_pairsInUse.find(pair) != _pairsInUse.end()) {

    std::ostringstream msg;
    msg << "Webserv does not support multiple websites with the same IP:Port "
           "pair.\nCheck config file for duplicates of the following:\n"
        << pair;
    logging::log(logging::Error, msg.str());
    exit(1);
  }
  _pairsInUse.insert(make_pair(pair, true));
}

// Check that Port # is uint16_t.
// This is necessary because using values outside of this type
// could result in undefined behavior.

void Server::checkPort(const std::string &Str) {

  const int maxPortDigits = 5;
  const int maxPortValue = 65535;
  if (Str.length() < 1 || Str.length() > maxPortDigits) {
    const std::string msg(Str + " is not a valid port number");
    logging::log(logging::Error, msg);
    exit(1);
  }
  int port = std::atoi(Str.c_str());
  if (port < 0 || port > maxPortValue) {
    const std::string msg(Str + " is not a valid port number");
    logging::log(logging::Error, msg);
    exit(1);
  }
}

/**
 * \brief getListeningSocket() handles all the steps of creating
 * and setting up a socket to listen for new connections. The socket
 * fd it returns is fully functional and ready to be added to the Server's
 * pollfd vector and listenMap.
 *
 * This function is where socket(), setsockopt(), bind(), and listen()
 * are called via dditional wrapper functions.
 *
 * If no socket is found, or bind() fails on every available socket(),
 * this indicates unusuable data in the config file. An error logging
 * function is called with specific troubleshooting suggestions,
 * and the webserver exits.
 *
 * \param	Info	pre-filled addrinfo struct, built from IP:port pair
 * \param	Pair	associated Website's IP:port pair Listen struct
 *
 * \return	int		fd of listening socket, already successfully
 *					set to listen and bound to ip:port pair
 */

int Server::getListeningSocket(struct addrinfo *Info, const Listen &Pair) {

  int sock;
  bool foundSock = 0;
  const struct addrinfo *p;
  for (p = Info; p != NULL; p = p->ai_next) {
    sock = createSocket(p);
    if (sock == -1) {
      continue;
    }
    foundSock = 1;
    clearSocket(sock);
    if (bindToIP(sock, p) == -1) {
      continue;
    }
    break;
  }
  if (p == NULL) {
    int error = errno;
    if (foundSock == 0) {
      handleSocketFailure(Pair, error);
    } else {
      handleBindFailure(Pair, error);
    }
    freeaddrinfo(Info);
    exit(1);
  }
  setToListen(sock);
  logging::log(logging::Debug, "server found socket. bind: success");
  return (sock);
}
