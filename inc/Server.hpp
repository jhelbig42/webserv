#pragma once

#include "Config.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
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
	std::map<int, Connection> cMap;
	std::vector<pollfd> fds;
	std::map<int, Website*> listenMap;

	void pollLoop(void);
};
