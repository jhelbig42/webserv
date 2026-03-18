#include "Server.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
// #include "NetworkingDefines.hpp" // Can be removed?
#include <cerrno>  // for errno
#include <cstdlib> // exit()
#include <cstring> // for strerror
#include <exception>
#include <fcntl.h> // TODO delete before submission, only for debugging
#include <map>
#include <ostream>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

void checkPort(std::string str);
static int clearSocket(const int Sock);
static int bindToIP(const int Sock, const struct addrinfo *p);
static int createSocket(const struct addrinfo *P);


void Server::initNetworking(const std::list<Website> &Websites) {

  // iterate through each website from config file
  for (std::list<Website>::const_iterator itW = Websites.begin();
       itW != Websites.end(); itW++) {
    const std::list<Listen> interfaces = itW->getInterfaces();

    // iterate through each IP:port pair of a given website,
	// contained within the Website's Listen struct
    for (std::list<Listen>::const_iterator itI = interfaces.begin();
         itI != interfaces.end(); itI++) {
      int sock = initListeningSocket(*itI, *itW);
      sMap.insert(std::make_pair(sInfo.fd, sInfo));
      const pollfd newFd = {sInfo.fd, POLLIN, 0};
      fds.push_back(newFd);
	  }
  }
}

// Initialize the creation of a listening socket for an IP:port pair

int Server::initListeningSocket(const Listen &Pair,
                                   const Website &Web) {
  checkPair(Pair);
  checkPort(Pair.port);
  struct addrinfo *serverInfo = getInfo(Interface);
  SocketInfo sInfo = getListeningSocket(serverInfo, Web, Interface);
  freeaddrinfo(serverInfo);
  return (sInfo);
}


// Checks that IP:port pair is not already being used by another website.
// NGINX allows multiple websites to use the same pair -- It is our
// choice to disallow it.

void checkPair(const Listen &Pair){

	std::string pair = Pair.ip + ":" + Pair.port;
	if (pairsInUse.find(pair) == pairsInUse.end()){
  		logging::log(logging::Debug, "Webserv does not support multiple websites with the same IP:Port pair. Check config file."
	exit(1);
  }
  pairsInUse.insert(pair);
}


// Check that Port # is uint16_t. Using values outside of this type
// could result in undefined behavior.

void checkPort(std::string str){
	
	if (str.length() < 1 || str.length()> 5){
		const std::string msg(str + " is not a valid port number");
		logging::log(logging::Error, msg);
		exit(1);
  }
	int port = std::atoi(str.c_str());
	if 	(port < 0 || port > 65535){
		const std::string msg(str + " is not a valid port number");
		logging::log(logging::Error, msg);
		exit(1);
	}
}


// Wrapper for getaddrinfo()
struct addrinfo *Server::getAddrInfo(const Listen &Interface) {

  struct addrinfo *info;
  logging::log2(logging::Debug, "getInfo() called with ip: ", Interface.ip);
  const int ret = getaddrinfo(Interface.ip.c_str(), Interface.port.c_str(), &_hints, &info);
  if (ret != 0) {
    const std::string msg(gai_strerror(ret));
    throw std::runtime_error("getaddrinfo: " + msg);
  }
  // logging::log(logging::Debug, addrinfoToStr(info, "server_info:"));
  return (info);
}

int Server::getListeningSocket(struct addrinfo *Info, const Website &Web, const Listen &Interface) {

  int sock;
  const struct addrinfo *p;
  for (p = Info; p != NULL; p = p->ai_next) {
    sock = createSocket(p);
    if (sock == -1) {
      continue;
    }
    clearSocket(sock);
    if (bindToIP(sock, p) == -1) {
      continue;
    }
    break;
  }
  if (p == NULL) {
    int error = errno;
    handleBindFailure(Info, Interface, error);
    freeaddrinfo(Info);
    exit(1);
  }
  logging::log(logging::Debug, "sever found socket. bind: success");
  return (socket);
}


static int clearSocket(const int Sock) {
  int yes = 1;
  const int ret = setsockopt(Sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (ret == -1) {
    std::ostringstream msg;
    msg << "setsockopt: " << std::strerror(errno);
    logging::log(logging::Error, msg.str());
  }
  return (ret);
}

static int bindToIP(const int Sock, const struct addrinfo *p) {

  const int ret = bind(Sock, p->ai_addr, p->ai_addrlen);
  if (ret != 0) {
    close(Sock);
    std::string msg(gai_strerror(ret));
    logging::log(logging::Info,
                 "bind: " + msg + " will continue trying sockets");
  }
  return (ret);
}

static int createSocket(const struct addrinfo *P) {
  const int sock = socket(P->ai_family, SOCK_STREAM | SOCK_NONBLOCK, 0);
  // 0 = TCP
  if (sock == -1) {
    std::ostringstream msg;
    msg << "socket: " << std::strerror(errno)
        << " (will continue trying sockets)";
    logging::log(logging::Info, msg.str());
  }
  //networking::printFcntlFlags(sock);
  return (sock);
}
