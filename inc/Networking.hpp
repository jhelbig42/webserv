#pragma once

#include "Connection.hpp"
#include "Config.hpp"
#include "Logging.hpp"
// #define _GNU_SOURCE // for extra poll() macros // defined elsewhere? Compiler complains
#include <cerrno>  // for errno
#include <cstdlib> // for exit
#include <cstring> // for memset
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdexcept> // for runtime_error
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <unistd.h> // for close
#include <map>

namespace networking {

struct client_addr {

	struct sockaddr_storage addr;
	int client_sock;
	socklen_t addr_size;
	// char client_IP[INET6_ADDRSTRLEN]; // Beej uses this - do we need it?
};

void start(void);

// Networking_init_server.cpp
struct addrinfo *get_server_info(void);

// Networking_init_socket.cpp
int get_server_socket(struct addrinfo *server_info);
void set_to_listen(const int sock);

// Networking.cpp (for now)
void poll_loop(int sock);
void process(int listen_sock, std::map<int, Connection> &c_map, std::vector<pollfd> &fds);
int accept_connection(int listen_sock, struct client_addr *candidate);
void add_connection_to_map(struct client_addr &candidate, std::map<int, Connection> &c_map);
void read_data(int listen_sock, int client_sock, Connection  &connection,  std::vector<pollfd> &fds);
} // namespace networking

