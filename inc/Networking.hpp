/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 18:01:57 by hallison          #+#    #+#             */
/*   Updated: 2026/02/25 13:44:19 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
// #define _GNU_SOURCE // for extra poll() macros // defined elsewhere? Compiler
// complains
#include <map>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace networking {

struct ClientAddr {

  struct sockaddr_storage addr;
  int clientSock;
  socklen_t addrSize;
  // char client_IP[INET6_ADDRSTRLEN]; // Beej uses this - do we need it?
};

void start(void);

// Networking_init_server.cpp
struct addrinfo *getServerInfo(void);

// Networking_init_socket.cpp
int getServerSocket(struct addrinfo *server_info);
void setToListen(const int sock);

// Networking_run.cpp
void pollLoop(const int sock);
void process(const int listen_sock, std::map<int, Connection> &c_map,
             std::vector<pollfd> &fds);
int acceptConnection(const int listen_sock, struct ClientAddr *candidate);
void addConnectionToMap(const struct ClientAddr &candidate,
                           std::map<int, Connection> &c_map);

void handlePollnval(int fd, std::map<int, Connection> &c_map);
void handlePollerr(int fd, std::map<int, Connection> &c_map);
void handlePollin(int fd, std::map<int, Connection> &c_map,
                   const int &listen_sock, std::vector<pollfd> &new_fd_batch);
void handlePollout(int fd, std::map<int, Connection> &c_map,
                   const int &listen_sock, std::vector<pollfd> &new_fd_batch);

// temporary Debug
// TODO delete or comment out before submission:
void printFcntlFlags(const int Sock);

} // namespace networking
