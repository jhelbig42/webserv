/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/10 18:38:14 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring> // for memset
#include <cstdlib> // for exit

#define PORT "80"

struct addrinfo	create_hints(void);
struct addrinfo *get_server_info(void);
void	print_addrinfo(struct addrinfo *info);
void    fill_addrinfo(char *node, struct addrinfo *hints, struct addrinfo *info);

void	init_networking(void){

	int sock;			// socket fd, listens for new connections
	Connection connection[10];	// naive array of < 10 connections -- will be container?
	struct addrinfo *server_info;
	
	server_info = get_server_info();

/*
	struct addrinfo *temp;
	int ret = -1;
	for (temp = info; temp != NULL, temp = temp->ai_next) {
		// add debug attempting to bind
		status = bind(sd, temp->ai_addr, temp->ai_addrlen);
		if (ret != 0) {
			std::cerr << "error in bind " << "\n" std::cout;
			// TODO throw exception and log error
			exit (1);
		}
		temp = temp->ai_next;
	}
*/
	freeaddrinfo(server_info);
	(void) sock;
}


struct addrinfo	create_hints(void){

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints); // init struct to empty;
	hints.ai_family = AF_UNSPEC; // allows either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // for TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // autofill IP	
	return (hints);
}

void	print_addrinfo(struct addrinfo *info){

        std::cout << "ai_flags = " << info->ai_flags << "\n";
        std::cout << "ai_family = " << info->ai_family << "\n";
        std::cout << "ai_socktype = " << info->ai_socktype << "\n";
        std::cout << "ai_protocol = " <<  info->ai_protocol << "\n";
        std::cout << "ai_addrlen = " << info->ai_addrlen << "\n";

}

struct addrinfo	*get_server_info(void){

		struct addrinfo hints = create_hints();
		struct addrinfo *info;

		int ret = getaddrinfo(NULL, PORT, &hints, &info); // NULL = localhost
        	if (ret != 0) {
			// fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			// TODO call error logging and throw exception
			std::cerr << "getaddrinfo: " << gai_strerror(ret);
			exit(1);
		}
		return (info);
}

