/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/11 15:44:44 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void	networking::start(void){

	int sock;						// socket fd, listens for new connections
	Connection connection[10];		// naive array of < 10 connections
	struct addrinfo *server_info;
	
	server_info = get_server_info();
/*
	struct addrinfo *temp;
	int ret = -1;
	for (temp = server_info; temp != NULL; temp = temp->ai_next) {
		// add debug attempting to bind
		ret = bind(socket, temp->ai_addr, temp->ai_addrlen);
		if (ret != 0) {
			std::cerr << "error in bind " << "\n";
			// TODO throw exception and log error
			exit (1);
		}
		temp = temp->ai_next;
	}
*/
	freeaddrinfo(server_info);
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

// This debug print function is a temporary solution.
// Rather than printing directly, I would like to be able to pass
// all of the debug text below as a single string to logging::log.
// This is tricky however because macros are being printed.
// Their values are correctly interpreted when redirected to
// std::cout, but I don't know how to preserve this when
// converting to a string. String stream needed?

void	networking::print_addrinfo_str(struct addrinfo *info){
		std::cout << "ai_flags = " << info->ai_flags << "\n";
        std::cout << "ai_family = " << info->ai_family << "\n";
        std::cout << "ai_socktype = " << info->ai_socktype << "\n";
        std::cout << "ai_protocol = " <<  info->ai_protocol << "\n";
        std::cout << "ai_addrlen = " << info->ai_addrlen << "\n";
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
			if (config::getLogLevel() <= logging::Debug){
				print_addrinfo_str(info);
			}
		}
		return (info);
}

