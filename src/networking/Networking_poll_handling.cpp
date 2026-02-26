/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking_poll_handling.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:55:29 by hallison          #+#    #+#             */
/*   Updated: 2026/02/25 17:46:41 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Logging.hpp"
#include "Networking.hpp"
#include "NetworkingDefines.hpp" // can be removed?
#include <map>
#include <ostream>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

void networking::handlePollnval(int fd, std::map<int, Connection> &c_map);
void networking::handlePollerr(int fd, std::map<int, Connection> &c_map);
void networking::handlePollin(int fd, std::map<int, Connection> &c_map,
                 const int &listen_sock, std::vector<pollfd> &newFdBatch);

void networking::handleTerminalCondition(const short revents, int fd, std::map<int, Connection> &c_map) {
    if (revents & POLLNVAL) {
      handlePollnval(fd, c_map);
      exit(1);
    }
    if (revents & POLLERR) {
      handlePollerr(fd, c_map);
      exit(1);
    }
    if (revents & POLLRDHUP) {
      handlePollrdhup(fd, c_map);
    }
    if (revents & POLLHUP) {
      logging::log2(logging::Debug, "Hangup from fd ", fd);
      exit(1);
    }
    if (revents & POLLPRI) {
      logging::log2(logging::Debug, "POLLPRI from fd ", fd);
      exit(1);
    }
}


void networking::handlePollnval(int fd, std::map<int, Connection> &c_map) {
  logging::log2(logging::Error,
                "networking::process(): POLLNAL.\n\tpoll() tried to read "
                "invalid fd. fd = ",
                fd);
  try {
    c_map.at(fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handlePollrdhup(int fd, std::map<int, Connection> &c_map) {
  logging::log2(logging::Debug,
                "networking::process(): POLLRDHUP.\n\t client hung up."
                " fd = ",
                fd);
  try {
    c_map.at(fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handlePollerr(int fd, std::map<int, Connection> &c_map) {
  const std::ostringstream msg;
  logging::log2(logging::Error,
                "networking::process(): POLLERR \n\tclient may have "
                "disconnected abruptly using RST, or socket is broken.\n fd = ",
                fd); // downgrade to Warning?
  try {
    c_map.at(fd).scheduleForDemolition();
  } catch (std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handlePollin(int fd, std::map<int, Connection> &c_map,
                              const int &listen_sock,
                              std::vector<pollfd> &newFdBatch) {
  logging::log2(logging::Debug, "POLLIN: fd ", fd);
  if (fd == listen_sock) { // listening socket got new connection
    ClientAddr candidate;
    if (acceptConnection(listen_sock, &candidate) != -1) {
      addConnectionToMap(candidate, c_map);
      const short events = POLLIN | POLLERR | POLLHUP | POLLNVAL | POLLPRI | POLLRDHUP;
      const pollfd newFd = {candidate.clientSock, events, 0};
      newFdBatch.push_back(newFd);
    }
  } else {
    const std::map<int, Connection>::iterator itC = c_map.find(fd);
    if (itC != c_map.end()) {

      // simple dummy
    //  (itC->second).readData();

      (itC->second).addToConditions(SockRead);

    } else {
      logging::log(logging::Error, "process: Connection not found in map "
                                   "container (This should never happen)");
      // could be removed after thorough testing
    }
  }
}

void networking::handlePollout(int fd, std::map<int, Connection> &c_map,
                               const int &listen_sock,
                               std::vector<pollfd> &newFdBatch) {
  logging::log2(logging::Debug, "POLLOUT: fd ", fd);
  const std::map<int, Connection>::iterator itC = c_map.find(fd);
  if (itC != c_map.end()) {

    logging::log2(logging::Debug, "Ready to send to ", fd);
    //send(fd, "We got your request", 1024, MSG_DONTWAIT);
    // eventual implementation
    /*
      (itC->second).addToConditions(SockWrite);
      (itC->second).serve(MAX_REQUEST);
    */
  } else {
    logging::log(logging::Error, "process: Connection not found in map "
                                 "container (This should never happen)");
    // could be removed after thorough testing
  }
}
