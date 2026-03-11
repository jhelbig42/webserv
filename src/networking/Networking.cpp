/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/03/11 14:42:56 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "Server.hpp"
// #define _GNU_SOURCE // for extra poll() macros // defined elsewhere? Compiler
// complains
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

/*
	TODO when connecting to config
  	get list of interface::port pairs from config parsing
  	for EACH of them...
  		generate addrinfo
		get socket
		free addrinfo
		set socket to listen
*/

/*
void generateDummyWebsite(std::string ip; Type type, unsigned int port) {
}
*/

void networking::start(void) {

  struct addrinfo *serverInfo = getServerInfo();
  int sock;
  sock = getServerSocket(serverInfo);
  freeaddrinfo(serverInfo);
  setToListen(sock);

  // signal handling goes here

  pollLoop(sock);
}
