/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking_poll_handling.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:55:29 by hallison          #+#    #+#             */
/*   Updated: 2026/02/23 16:52:20 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void handle_pollnval(int fd, std::map<int, Connection> &c_map);
void handle_pollerr(int fd, std::map<int, Connection> &c_map);
void handle_pollin(int fd, std::map<int, Connection> &c_map,
                   const int &listen_sock, std::vector<pollfd> &new_fd_batch);


void networking::handle_pollnval(int fd, std::map<int, Connection> &c_map) {
  logging::log2(logging::Error,
                "networking::process(): POLLNAL.\n\tpoll() tried to read "
                "invalid fd. fd = ",
                fd);
  try {
    c_map.at(fd).schedule_for_demolition();
  } catch (const std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handle_pollerr(int fd, std::map<int, Connection> &c_map) {
  std::ostringstream msg;
  logging::log2(logging::Error,
                "networking::process(): POLLERR \n\tclient may have "
                "disconnected abruptly using RST, or socket is broken.\n fd = ",
                fd); // downgrade to Warning?
  try {
    c_map.at(fd).schedule_for_demolition();
  } catch (std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handle_pollin(int fd, std::map<int, Connection> &c_map,
                               const int &listen_sock,
                               std::vector<pollfd> &new_fd_batch) {
  logging::log2(logging::Debug, "POLLIN: fd ", fd);
  if (fd == listen_sock) { // listening socket got new connection
    client_addr candidate;
    if (accept_connection(listen_sock, &candidate) != -1) {
      add_connection_to_map(candidate, c_map);
      pollfd new_fd = {candidate.client_sock, POLLIN, 0};
      new_fd_batch.push_back(new_fd);
    }
  }
  else {
    std::map<int, Connection>::iterator c_it = c_map.find(fd);
    if (c_it != c_map.end()) {
      (c_it->second).read_data();
    } else {
      logging::log(logging::Error, "process: Connection not found in map "
                                   "container (This should never happen)");
      // could be removed after thorough testing
    }
  }
}
