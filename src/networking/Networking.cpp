/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/24 13:07:12 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "Networking.hpp"
// #include "NetworkingDefines.hpp" // can be deleted?
// #define _GNU_SOURCE // for extra poll() macros // defined elsewhere? Compiler
// complains
#include <cerrno>  // for errno
#include <cstdlib> // for exit
#include <cstring> // for memset
#include <iostream>
#include <map>
#include <netdb.h>
#include <poll.h>
#include <sstream>
#include <stdexcept> // for runtime_error
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // for close

void networking::start(void) {

  struct addrinfo *serverInfo = getServerInfo();
  int sock;
  sock = getServerSocket(serverInfo);
  freeaddrinfo(serverInfo);

  setToListen(sock);

  // signal handling goes here

  pollLoop(sock);
}
