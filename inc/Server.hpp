#pragma once

#include "Config.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "Website.hpp"
#include <map>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

struct ClientAddr {
  struct sockaddr_storage addr;
  int clientSock;
  socklen_t addrSize;
  // char client_IP[INET6_ADDRSTRLEN];
  // Beej uses this - do we need it?
};

class Server {
	
	public:
	
	// Server.cpp
	explicit Server(const std::list<Website>  &websites);
	~Server();
	bool socketIsListener(int Fd);
	bool socketIsClient(int Fd);
	
	// ServerRun.cpp
	void pollLoop(void);


	private:

	// variables
	addrinfo _hints; // our specifications for getaddrinfo
	std::vector<pollfd> _fds;
	std::map<int, const Website*> _listenMap;
	std::map<int, Connection> _clientMap;
	std::map<int, Connection&> _cgiWriteMap;
	std::map<int, Connection&> _cgiReadMap;
	std::vector<pollfd> _newFdBatch;
	std::map<std::string, bool> _pairsInUse; // listening <IP:Port>

	// ServerInit.hpp -- set up listening sockets, pollfds, listenMap
	void initNetworking(const std::list<Website> &Websites);
	void checkPair(const Listen &Pair);
	void checkPort(const std::string &str);
	void initListeningSocket(const Listen &Pair, const Website &Web);
	int getListeningSocket(struct addrinfo *Info, const Listen &Pair);

	// ServerWrappers.hpp -- networking wrappers
	struct addrinfo *getAddrInfo(const Listen &Pair); // getaddrinfo()
	int createSocket(const struct addrinfo *P); // socket()
	int clearSocket(const int Sock); // setsockopt()
	int bindToIP(const int Sock, const struct addrinfo *p); //bind()
	void setToListen(const int Sock); // listen()
	int acceptConnection(int ListenerFd, ClientAddr *Candidate); //accept()
	
	// ServerRun.hpp -- outer loop function PollLoop() is public
	void process(void);
	void addConnectionToMap(int ListenerFd, const struct ClientAddr &Candidate);

	// ServerHandlePoll.hpp
	bool reventsAreTerminal(int revents);
	void handleTerminalCondition(struct pollfd &polled);
	void handleServableCondition(struct pollfd &polled);
	
	// ServerHandlePollErrs.hpp
	void handlePollnval(int Fd);
	void handlePollerr(int Fd);
	void handlePollhup(int Fd);

	// ServerHandlPollin.hpp
	void handlePollin(int Fd);

	// ServerHandlePollout.hpp
	void handlePollout(int Fd);

	// ServerErrorHandling.hpp
	void handleSocketFailure(const Listen &Interface, const int Error);
	void handleBindFailure(const Listen &Interface, const int Error);

	// ServerDebug.hpp
	std::string addrinfoToStr(const struct addrinfo *Info, const std::string &Msg);
	std::string interfaceInfoToStr(const Listen &Interface);
	void printFcntlFlags(const int Sock);

};
