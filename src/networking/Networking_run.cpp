/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking_run.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 16:36:50 by hallison          #+#    #+#             */
/*   Updated: 2026/02/23 18:50:52 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

// TODO These functions could be made static:
// process(), acceptConnection(), addConnectionToMap()
// But then they would lose sight of networking::client_addr struct
// and netoworking::read_data().
// Decide on most elegant arrangement.

void networking::pollLoop(const int sock);
void networking::process(const int listen_sock,
                         std::map<int, Connection> &c_map,
                         std::vector<pollfd> &fds);
int networking::acceptConnection(const int listen_sock,
                                  client_addr *candidate);
void networking::addConnectionToMap(const struct client_addr &candidate,
                                       std::map<int, Connection> &c_map);

// pollLoop() introduces the while(1) networking loop that will run
// for the duration of the webserver. This function:
//
// • Declares c_map, a map of clients (key = fd, value = Connection object)
// • Declares fds, a vector of pollfd structs
//  - Pollfd structs are library-defined and required by poll()
//  - Placing pollfds in a vector is useful for constant re-sizing,
//    as well as keeping constant track of the collection size.
//    Can ALSO be used as a normal array, which poll() expects.
// • Adds the server's listening socket as the first member of pollfd vector
// • Enters the polling loop, which
// 	- continuously checks and marks which pollfds are ready for I/O
// 	- processes the ones that are marked

void networking::pollLoop(const int sock) {

  std::map<int, Connection> c_map;
  std::vector<pollfd> fds;

  pollfd listener = {sock, POLLIN, 0};
  fds.push_back(listener);

  while (1) {
    int res = poll(fds.data(), (nfds_t)fds.size(),
                   -1); // without restriction to fds.size this cast is unsafe
    if (res == -1) {
      std::ostringstream msg;
      msg << "poll: " << std::strerror(errno);
      logging::log(logging::Error, msg.str());

      // TODO How to best handle poll() failure?
      // Currently, logging error and exiting
      // Could also log Warning and continue.
    }
    process(sock, c_map, fds);
  }
}

// process() iterates through the vector of fds and handles the flags
// that were set by poll() in fd[i]->events & fd[i]->revents.
//
// If one of the handling functions sets the Connection's _delete field
// to true, due to client hang-up or error, the fd is closed, 
// and both the Connection & the fd are deleted.
//
// If a new connection is discovered, a new Connection and fd
// are added.
//
// TODO Handle additional flags

void networking::process(const int listen_sock,
                         std::map<int, Connection> &c_map,
                         std::vector<pollfd> &fds) {

  logging::log(logging::Debug, "Process()");
  std::vector<pollfd> new_fd_batch;
  for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end();) {
    if (it->revents & POLLNVAL) {
      handlePollnval(it->fd, c_map);
      exit(1);
    }
    if (it->revents & POLLERR) {
      handlePollerr(it->fd, c_map);
      exit(1);
    }
    if (it->revents & POLLHUP) {
      logging::log2(logging::Debug, "Hangup from fd ", it->fd);
      exit(1);
    }
    if (it->revents & POLLIN) { // data to read | hang-up
      handlePollin(it->fd, c_map, listen_sock, new_fd_batch);
    }
    // CHECK IF CONNECTION SHOULD BE DELETED
    if (it->fd != listen_sock && c_map.at(it->fd)._delete == true) {
	  close(it->fd);
      c_map.erase(it->fd);
      it = fds.erase(it);
    } else {
      it++;
    }
  }
  fds.insert(fds.end(), new_fd_batch.begin(), new_fd_batch.end());
}

// acceptConnections() is a a wrapper for accept(), which extracts
// the first connection request on the queue of pending connections
// for the listening socket. Creates new socket for the
// incoming connection.
//
// RETURNS: fd for new socket

int networking::acceptConnection(const int listen_sock,
                                  client_addr *candidate) {

  candidate->clientSock = accept(
      listen_sock, (struct sockaddr *)&candidate->addr, &candidate->addrSize);
  if (candidate->clientSock == -1) {
    std::ostringstream msg;
    msg << "accept: " << std::strerror(errno)
        << " (can continue trying to accept connections)";
    logging::log(logging::Error,
                 msg.str()); // may downgrade log level at some point
    return (-1);
  }
  logging::log2(logging::Debug, "Connection accepted on socket ", candidate->clientSock);
  return (0);
}

void networking::addConnectionToMap(const struct client_addr &candidate,
                                       std::map<int, Connection> &c_map) {

  Connection new_connection =
      Connection(candidate.clientSock, candidate.addr, candidate.addrSize);
  c_map.insert(std::make_pair(candidate.clientSock, new_connection));
}
