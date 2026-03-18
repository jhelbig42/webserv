#pragma once

#include "Config.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
//#include "Socket.hpp"
#include "Website.hpp"
// #define _GNU_SOURCE // for extra poll() macros // defined elsewhere? Compiler
// complains
#include <map>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

struct SocketInfo {
  const int fd;
  enum { LISTEN, CLIENT } type; 
  union
  {
	std::vector<Website> *const website; // for LISTEN type
	Connection const *connection; // for CLIENT type
  };
};

class Server {

	private:

	addrinfo _hints;

	// init Networking
	void initNetworking(const std::list<Website> &Websites);
	SocketInfo initListeningSocket(const Listen &Interface, const Website &Web);
	struct addrinfo *getInfo(const Listen &Interface);
	struct addrinfo createHints(void);
	SocketInfo getListeningSocket(struct addrinfo *Info, const Website &Web, const Listen &Interface);

	// debug
	void handleBindFailure(const struct addrinfo *Info, const Listen &Interface, const int Error);
	static std::string addrinfoToStr(const struct addrinfo *Info, const std::string &Msg);
	

	public:
	Server(const std::list<Website>  &websites);
	~Server();
	void pollLoop(void);
	
	std::vector<pollfd> fds;
	std::map<int, std::vector<const Website *> > listenMap;
	std::map<int, Connection*> clientMap;
};
