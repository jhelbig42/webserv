/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/13 16:10:44 by hallison         ###   ########.fr       */
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

void networking::read_data(int listen_sock, int client_sock,
                           Connection &connection, std::vector<pollfd> &fds) {

  // client_sock is contained in the containers but let's
  // see if we get rid of them here

  ssize_t bytes_read =
      recv(client_sock, &connection.read_buf[0], MAX_REQUEST, 0);
  (void)bytes_read;
}


