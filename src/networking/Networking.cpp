/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/24 14:06:03 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
// #define _GNU_SOURCE // for extra poll() macros // defined elsewhere? Compiler
// complains
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

void networking::start(void) {

  struct addrinfo *serverInfo = getServerInfo();
  int sock;
  sock = getServerSocket(serverInfo);
  freeaddrinfo(serverInfo);

  setToListen(sock);

  // signal handling goes here

  pollLoop(sock);
}
