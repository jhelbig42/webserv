/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/13 13:06:21 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void networking::start(void) {

  struct addrinfo *server_info = get_server_info();
  int sock;

//  *server_info = get_server_info();
  sock = get_server_socket(server_info);
  freeaddrinfo(server_info);
  set_to_listen(sock);
  
  // signal handling goes here?
 
  accept_clients(sock);
}

void	networking::accept_clients(int sock){
  
  	Connection connection[BACKLOG]; // naive array of connections
	static int active_clients; // counter for # of active clients, temp solution
	struct sockaddr_storage addr;
	socklen_t	addr_size;
	int		client_sock;

	while (1) {
		
		addr_size = sizeof addr;
		client_sock = accept(sock, (struct sockaddr *)&addr, &addr_size);
		if (client_sock == -1) {
    		std::ostringstream msg;
    		msg << "accept: " << std::strerror(errno) <<
				" (will continue trying to accept connections";
  			logging::log(logging::Error, msg.str());
			continue;
		}
		active_clients++;
		std::cout << "Webserv got connection\n";
		std::cout << "Number of active clients: " << active_clients << "\n";
		std::cout << "<process request>\n";
		std::cout << "<response>\n";
		close(client_sock);
		std::cout << "Client sock closed\n";
	}
	(void)active_clients;
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
