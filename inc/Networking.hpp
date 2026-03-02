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
int getServerSocket(struct addrinfo *Server_info);
void setToListen(const int Sock);

// Networking_run.cpp
void pollLoop(const int Sock);
void process(const int ListenSock, std::map<int, Connection> &CMap,
             std::vector<pollfd> &Fds);
int acceptConnection(const int ListenSock, struct ClientAddr *Candidate);
void addConnectionToMap(const struct ClientAddr &Candidate,
                           std::map<int, Connection> &CMap);

void handlePollnval(int Fd, std::map<int, Connection> &CMap);
void handlePollerr(int Fd, std::map<int, Connection> &CMap);
void handlePollin(int Fd, std::map<int, Connection> &CMap,
                   const int &listen_sock, std::vector<pollfd> &newFdBatch);
void handlePollout(int Fd, std::map<int, Connection> &CMap);
void handlePollrdhup(int Fd, std::map<int, Connection> &CMap);
void handleTerminalCondition(const short Revents, const int Fd, std::map<int, Connection> &CMap);
void handleServableCondition(const int ListenSock, const short Revents, const int Fd, std::map<int, Connection> &CMap, std::vector<pollfd> &newFdBatch);

// temporary Debug
// TODO delete or comment out before submission:
void printFcntlFlags(const int Sock);

} // namespace networking
