/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/17 19:12:55 by hallison         ###   ########.fr       */
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

  poll_loop(sock);
}
