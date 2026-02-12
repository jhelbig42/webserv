/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/12 15:15:42 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void	networking::start(void){

	Connection connection[10];		// naive array of < 10 connections
	struct addrinfo *server_info =  get_server_info();
	int sock = get_server_socket(server_info); 
	freeaddrinfo(server_info);
	// add throw in get_server_socket		
	(void) sock;
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

int networking::create_socket(struct addrinfo *server_info, struct addrinfo *p){
		
		int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol); // could set ai_protocol manually to TCP?
		if (sock == -1){
			std::ostringstream msg;
			msg << "socket: " << std::strerror(errno)
				<< " (will continue trying sockets)";
			logging::log(msg.str(), logging::Info);
		}
		return (sock);
}

int	networking::get_server_socket(struct addrinfo *server_info){
	
	int sock;
	struct addrinfo *p;
	for (p = server_info; p != NULL; p = p->ai_next) {
		sock = create_socket(server_info, p);
		if (sock == -1){
			continue;
		}
		// I believe this function is used to clear a previously-
		// used port for re-use. Leaving it out for now to test.
		/*
		int yes = 1;
		ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
			&yes, sizeof(int));
		if (ret == -1) {
				std::cerr << "setsockopt: " << gai_strerror(ret) << "\n";
				exit(1);
		}
		*/

		int ret = bind(sock, p->ai_addr, p->ai_addrlen);
		if (ret != 0) {
			close(sock);
			std::string msg (gai_strerror(ret));
			logging::log("bind: " + msg + "will continue trying sockets",
				logging::Info);
			continue;
		}
		logging::log("bind: success!", logging::Debug);
		break; // necessary?
	}
	if (p == NULL){
		std::cout << "server failed to find a socket\n";
		return (-1);
	}
	return (sock);
}
