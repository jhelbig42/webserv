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

// TODO change initListeningSocket and getListeningSocket to return
// a socklen_t when positive -1 isn't needed / check that casting
// from the necessary int isn't more work thatn it's worth

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
	int initListeningSocket(const Listen &Pair, const Website &Web);
	struct addrinfo *getAddrInfo(const Listen &Interface);
	struct addrinfo createHints(void);
	int getListeningSocket(struct addrinfo *Info, const Website &Web, const Listen &Interface);

	// debug
	void handleBindFailure(const struct addrinfo *Info, const Listen &Interface, const int Error);
	static std::string addrinfoToStr(const struct addrinfo *Info, const std::string &Msg);
	

	public:
	Server(const std::list<Website>  &websites);
	~Server();
	void pollLoop(void);
	void checkPair(const Listen &Pair);
	void checkPort(std::string str);

	std::vector<pollfd> fds;
	//std::map<int, std::vector<const Website *> > listenMap;
	std::map<int, const Website> listenMap;
	std::map<int, Connection*> clientMap;
	std::map<std::string, bool> pairsInUse; // listening <IP:Port>
};
