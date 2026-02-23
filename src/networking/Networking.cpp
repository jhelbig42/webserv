/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/23 18:44:54 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void networking::start(void) {

  struct addrinfo *server_info = getServerInfo();
  int sock;
  sock = getServerSocket(server_info);
  freeaddrinfo(server_info);

  setToListen(sock);

  // signal handling goes here

  pollLoop(sock);
}
