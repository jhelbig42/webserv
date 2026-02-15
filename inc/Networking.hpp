#pragma once

#include "Connection.hpp"
#include "Config.hpp"
#include "Logging.hpp"
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

void start(void);
struct addrinfo create_hints(void);
struct addrinfo *get_server_info(void);
std::string get_addrinfo_str(struct addrinfo *info, std::string msg);
void print_addrinfo_str(struct addrinfo *info);
void fill_addrinfo(char *node, struct addrinfo *hints, struct addrinfo *info);
int get_server_socket(struct addrinfo *server_info);
int create_socket(struct addrinfo *server_info, struct addrinfo *p);
int clear_socket(int sock);
int bind_to_ip(int sock, struct addrinfo *p);
void set_to_listen(int sock);
void accept_clients(int sock);
void process(int sock, int *fd_count, std::vector<pollfd> &fds);
static Connection *create_connection(int sock); // attempts to accept() incoming connections. calls Connection constructor only if accept() is successful
} // namespace networking
