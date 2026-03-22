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
  // char client_IP[INET6_ADDRSTRLEN]; // Beej uses this - do we need it?
};

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
	
	public:
	explicit Server(const std::list<Website>  &websites);
	~Server();
	void pollLoop(void);
	void checkPair(const Listen &Pair);
	void checkPort(std::string str);

	std::vector<pollfd> fds;
	std::map<int, const Website*> listenMap;
	std::map<int, Connection*> clientMap; // TODO make non-pointer?
	std::map<std::string, bool> pairsInUse; // listening <IP:Port>
	std::vector<pollfd> newFdBatch;

	private:

	addrinfo _hints;

	// init Networking
	void initNetworking(const std::list<Website> &Websites);
	void initListeningSocket(const Listen &Pair, const Website &Web);
	struct addrinfo *getAddrInfo(const Listen &Interface);
	struct addrinfo createHints(void);
	int getListeningSocket(struct addrinfo *Info, const Website &Web, const Listen &Interface);
	
	// poll handling
	bool reventsAreTerminal(int revents);
	void handleTerminalCondition(struct pollfd &polled);
	void handlePollnval(int Fd);
	void handlePollerr(int Fd);
	void handlePollrdhup(int Fd);

	void handleServableCondition(struct pollfd &polled);
	void handlePollin(int Fd);
	void handlePollout(int Fd);

	// process
	void process(void);
	int acceptConnection(int ListenerFd, ClientAddr *Candidate);
	void addConnectionToMap(int ListenerFd, const struct ClientAddr &Candidate);

	// debug
	void handleBindFailure(const struct addrinfo *Info, const Listen &Interface, const int Error);
	static std::string addrinfoToStr(const struct addrinfo *Info, const std::string &Msg);
	void printFcntlFlags(const int Sock);	

};
