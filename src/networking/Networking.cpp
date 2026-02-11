/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/11 16:56:01 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void	networking::start(void){

	int sock;						// socket fd, listens for new connections
	Connection connection[10];		// naive array of < 10 connections
	struct addrinfo *server_info;
	
	server_info = get_server_info();
	struct addrinfo *p;
	int ret = -1;
	for (p = server_info; p != NULL; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock == -1){
		//	std::string msg (gai_strerror(ret));
		//	throw std::runtime_error("socket: " + msg);
			std::cerr << "socket: " << gai_strerror(ret) << "\n";
			continue;
		}
		/*
		int ret = bind(socket, p->ai_addr, p->ai_addrlen);
		if (ret != 0) {
			std::cerr << "error in bind " << "\n";
			// TODO throw exception and log error
			exit (1);
		}
		temp = temp->ai_next;
		*/
	}
	freeaddrinfo(server_info);
	(void) sock;
	(void) ret;
}


struct addrinfo	networking::create_hints(void){

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints); // init struct to empty;
	hints.ai_family = AF_UNSPEC; // allows either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // for TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // autofill IP	
	return (hints);
}


std::string	networking::get_addrinfo_str(struct addrinfo *info, std::string msg){
	std::ostringstream oss;
	oss << "\n\t" << msg << "\n"
		<< "\tai_flags = " << info->ai_flags << "\n"
		<< "\tai_family = " << info->ai_family << "\n"
		<< "\tai_socktype = " << info->ai_socktype << "\n"
		<< "\tai_protocol = " <<  info->ai_protocol << "\n"
		<< "\tai_addrlen = " << info->ai_addrlen << "\n";
		return (oss.str());
}


struct addrinfo	*networking::get_server_info(void){

		struct addrinfo hints = create_hints();
		struct addrinfo *info;

		int ret = getaddrinfo(NULL, PORT, &hints, &info); // NULL = localhost
        if (ret != 0) {
			std::string msg (gai_strerror(ret));
			throw std::runtime_error("getaddrinfo: " + msg);
		}
		else {
			logging::log("addrinfo server_info created", logging::Debug);
			logging::log(get_addrinfo_str(info, "server_info:"), logging::Debug);
		}
		return (info);
}

