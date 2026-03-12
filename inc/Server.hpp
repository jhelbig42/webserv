#pragma once

#include "Config.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "Socket.hpp"
#include "Website.hpp"
// #define _GNU_SOURCE // for extra poll() macros // defined elsewhere? Compiler
// complains
#include <map>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

class Server {

	public:
	//std::map<int, Connection> cMap;
	//std::vector<pollfd> fds;
	//std::map<int, Website*> listenMap;

	std::vector<Socket> sockets;
	Server(const std::list<Website>  &websites);
	void pollLoop(void);

	private:

	addrinfo _hints;

	// init Networking
	void initNetworking(const std::list<Website> &Websites);
	Socket initListeningSocket(const Listen &Interface, const Website &Web);
	struct addrinfo *getInfo(const Listen &Interface);
	struct addrinfo createHints(void);
	static std::string addrinfoToStr(const struct addrinfo *Info, const std::string &Msg);
};
