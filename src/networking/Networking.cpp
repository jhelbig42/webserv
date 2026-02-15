/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/13 16:10:44 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void networking::start(void) {

  struct addrinfo *server_info = get_server_info();
  int sock;
  sock = get_server_socket(server_info);
  freeaddrinfo(server_info);

  set_to_listen(sock);
  
  // signal handling goes here?
 
  accept_clients(sock);
}

void	networking::accept_clients(int sock){
  
	static int fd_count; // counter for # of active clients
	std::map<int, Connection> c_map; // empty map of Connection objects
	std::vector<pollfd> fds; // empty vector of poll_fd structs (see <poll.h>)
	
	pollfd listener = {sock, POLLIN, 0}; // create pollfd for listening socket
	fds.push_back(listener); // add to vector
	fd_count++;

	(void) c_map;

	while (1) {
		int res = poll(fds.data(), fd_count, -1); // poll indefinitely (allowed?)
		if (res == -1) {
			//logging::log(Error, "poll: ", std:sterror(errno)); // TODO after merge
			logging::log(logging::Error, "poll: ");
			exit(1); // Should exit or continue?
		}
		process(sock, c_map, &fd_count, fds);
	}
}

// This function follows Beej closely
void networking::process(int listen_sock, std::map<int, Connection> &c_map, int *fd_count, std::vector<pollfd> &fds){

	std::vector<pollfd> new_fd_batch;	
	for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++) {
		if (it->revents & POLLNVAL || it->revents & POLLERR) {
			logging::log(logging::Error, "polling file descriptor gave unexpected results");
			// TODO print fd after merge
			// TODO more robust and granular error-handling, see additional flags in
			// the man pages & double-check subject, re: errno reactions
			exit(1);
		}
		if (it->revents & (POLLIN | POLLHUP)) {
			// is there something to read, or did someone hang up on us?
			if (it->fd == listen_sock) {// listening socket got new connection
				
				client_addr candidate;
				if (accept_connection(listen_sock, &candidate) != -1) {
					add_connection_to_map(candidate, c_map);	
					pollfd new_fd = {listen_sock, POLLIN, 0};
					new_fd_batch.push_back(new_fd);
				}
			}
			else {
				handle_existing_connection(listen_sock, c_map, fd_count, fds);
			}
		}
	}
}


void	networking::handle_existing_connection(int listen_sock, std::map<int, Connection> &c_map, int *fd_count, std::vector<pollfd> &fds) {
	std::cout << "here client data is handled\n";
	exit(1);
}

int	networking::accept_connection(int listen_sock, client_addr *candidate){ 
	
	candidate->client_sock = accept(listen_sock, (struct sockaddr *)&candidate->addr, &candidate->addr_size);
	if (candidate->client_sock == -1){
		std::ostringstream msg;
		msg << "accept: " << std::strerror(errno) <<
			" (can continue trying to accept connections)";
		logging::log(logging::Error, msg.str()); // may downgrade log level at some point
		return (-1);
	}
	std::cout << "\nConnection accepted on socket " << candidate->client_sock << "\n\n"; // TODO move to debug
	return (0);
}

void networking::handle_new_connection(int listen_sock, std::map<int, Connection> &c_map, int *fd_count, std::vector<pollfd> &fds) {
	
	struct sockaddr_storage addr;
	int		client_sock;
	socklen_t addr_size = sizeof addr;

	client_sock = accept(listen_sock, (struct sockaddr *)&addr, &addr_size);
	if (client_sock == -1){
		std::ostringstream msg;
		msg << "accept: " << std::strerror(errno) <<
			" (can continue trying to accept connections)";
		logging::log(logging::Error, msg.str()); // may downgrade log level at some point
		return;
	}
	Connection new_connection = Connection(client_sock, addr, addr_size);
//	logging::log3(logging:Debug, "Connection accepted on socket ", _sock, "\n"); // TODO after merge
	c_map.insert(std::make_pair(new_connection.get_sock(), new_connection));
	
	pollfd new_fd = {client_sock, POLLIN, 0};
	//fds.push_back(new_fd);
	//return (new_fd);
	//*(fd_count)++;
	(void)fd_count;
	(void)new_fd;
}

void networking::add_connection_to_map(struct client_addr &candidate, std::map<int, Connection> &c_map) {
	
	Connection new_connection = Connection(candidate.client_sock, candidate.addr, candidate.addr_size);
	c_map.insert(std::make_pair(candidate.client_sock, new_connection));
}

void	networking::set_to_listen(int sock){
  
  if (listen(sock, BACKLOG == -1)) {
    	std::ostringstream msg;
    	msg << "listen: " << std::strerror(errno);
  		throw std::runtime_error(msg.str());
	}
  logging::log(logging::Debug, "server is listening for connections...");
}

struct addrinfo networking::create_hints(void) {

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints); // init struct to empty;
  hints.ai_family = AF_UNSPEC;     // allows either IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // for TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // autofill my IP
  return (hints);
}

std::string networking::get_addrinfo_str(struct addrinfo *info,
                                         std::string msg) {
  std::ostringstream oss;
  oss << "\n\t" << msg << "\n"
      << "\tai_flags = " << info->ai_flags << "\n"
      << "\tai_family = " << info->ai_family << "\n"
      << "\tai_socktype = " << info->ai_socktype << "\n"
      << "\tai_protocol = " << info->ai_protocol << "\n"
      << "\tai_addrlen = " << info->ai_addrlen << "\n";
  return (oss.str());
}

struct addrinfo *networking::get_server_info(void) {

  struct addrinfo hints = create_hints();
  struct addrinfo *info;

  int ret = getaddrinfo(NULL, PORT, &hints, &info); // NULL = localhost
  if (ret != 0) {
    std::string msg(gai_strerror(ret));
    throw std::runtime_error("getaddrinfo: " + msg);
  } else {
    logging::log(logging::Debug, "addrinfo server_info created");
    logging::log(logging::Debug, get_addrinfo_str(info, "server_info:"));
  }
  return (info);
}

int networking::create_socket(struct addrinfo *server_info,
                              struct addrinfo *p) {

  int sock = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol); // could set ai_protocol manually to TCP?
  if (sock == -1) {
    std::ostringstream msg;
    msg << "socket: " << std::strerror(errno)
        << " (will continue trying sockets)";
    logging::log(logging::Info, msg.str());
  }
  return (sock);
}

int networking::clear_socket(int sock) {
  int yes = 1;
  int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (ret == -1) {
    std::ostringstream msg;
    msg << "setsockopt: " << std::strerror(errno);
    logging::log(logging::Error, msg.str());
  }
  return (ret);
}

int networking::bind_to_ip(int sock, struct addrinfo *p) {

  int ret = bind(sock, p->ai_addr, p->ai_addrlen);
  if (ret != 0) {
    close(sock);
    std::string msg(gai_strerror(ret));
    logging::log(logging::Info,
                 "bind: " + msg + " will continue trying sockets");
  }
  return (ret);
}

int networking::get_server_socket(struct addrinfo *server_info) {

  int sock;
  struct addrinfo *p;
  for (p = server_info; p != NULL; p = p->ai_next) {
    sock = create_socket(server_info, p);
    if (sock == -1) {
      continue;
    }

    clear_socket(sock); // not necessary until we have multiple connections?

    if (bind_to_ip(sock, p) == -1) {
      continue;
    }
    break;
  }
  if (p == NULL) {
  	freeaddrinfo(server_info);
    throw std::runtime_error("server failed to find a socket");
  } else {
    logging::log(logging::Debug, "sever found socket. bind: success");
  }
  return (sock);
}

// to be deleted
/*
Connection *networking::create_connection(int server_sock){
	
	struct sockaddr_storage addr;
	int		client_sock;
	socklen_t addr_size = sizeof addr;

	client_sock = accept(server_sock, (struct sockaddr *)&addr, &addr_size);
	if (client_sock == -1){
		std::ostringstream msg;
		msg << "accept: " << std::strerror(errno) <<
			" (can continue trying to accept connections)";
		logging::log(logging::Error, msg.str()); // may downgrade log level at some point
		return (NULL);
	}
	Connection *new_connection = new Connection(client_sock, addr, addr_size);
	std::cout << "\nConnection accepted on socket " << client_sock << "\n\n";
//	logging::log3(logging:Debug, "Connection accepted on socket ", _sock, "\n");
	return (new_connection);
}
*/

