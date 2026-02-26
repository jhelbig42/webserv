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
void networking::handleTerminalCondition(const short Revents, const int Fd, std::map<int, Connection> &CMap);
void networking::handleServableCondition(const int ListenSock, const short Revents, const int Fd, std::map<int, Connection> &CMap, std::vector<pollfd> &newFdBatch);

void networking::handleTerminalCondition(const short Revents, const int Fd, std::map<int, Connection> &CMap) {
    if (Revents & POLLNVAL) {
      handlePollnval(Fd, CMap);
      exit(1);
    }
    if (Revents & POLLERR) {
      handlePollerr(Fd, CMap);
      exit(1);
    }
    if (Revents & POLLRDHUP) {
      handlePollrdhup(Fd, CMap);
    }
    if (Revents & POLLHUP) {
      logging::log2(logging::Debug, "Hangup from fd ", Fd);
      exit(1);
    }
    if (Revents & POLLPRI) {
      logging::log2(logging::Debug, "POLLPRI from fd ", Fd);
      exit(1);
    }
}

void networking::handleServableCondition(const int ListenSock, const short Revents, const int Fd, std::map<int, Connection> &CMap, std::vector<pollfd> &newFdBatch) {
      if (Revents & POLLIN) { // data to read | hang-up
        handlePollin(Fd, CMap, ListenSock, newFdBatch);
      }
      if (Revents & POLLOUT) {
        handlePollout(Fd, CMap, ListenSock, newFdBatch);
      }
}


void networking::handlePollnval(int Fd, std::map<int, Connection> &CMap) {
  logging::log2(logging::Error,
                "networking::process(): POLLNAL.\n\tpoll() tried to read "
                "invalid fd. fd = ",
                Fd);
  try {
    CMap.at(Fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handlePollrdhup(int Fd, std::map<int, Connection> &CMap) {
  logging::log2(logging::Debug,
                "networking::process(): POLLRDHUP.\n\t client hung up."
                " fd = ",
                Fd);
  try {
    CMap.at(Fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handlePollerr(int Fd, std::map<int, Connection> &CMap) {
  const std::ostringstream msg;
  logging::log2(logging::Error,
                "networking::process(): POLLERR \n\tclient may have "
                "disconnected abruptly using RST, or socket is broken.\n fd = ",
                Fd); // downgrade to Warning?
  try {
    CMap.at(Fd).scheduleForDemolition();
  } catch (std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

void networking::handlePollin(int Fd, std::map<int, Connection> &CMap,
                              const int &listen_sock,
                              std::vector<pollfd> &newFdBatch) {
  logging::log2(logging::Debug, "POLLIN: fd ", Fd);
  if (Fd == listen_sock) { // listening socket got new connection
    ClientAddr candidate;
    if (acceptConnection(listen_sock, &candidate) != -1) {
      addConnectionToMap(candidate, CMap);
      const short events = POLLIN | POLLERR | POLLHUP | POLLNVAL | POLLPRI | POLLRDHUP;
      const pollfd newFd = {candidate.clientSock, events, 0};
      newFdBatch.push_back(newFd);
    }
  } else {
    const std::map<int, Connection>::iterator itC = CMap.find(Fd);
    if (itC != CMap.end()) {

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

void networking::handlePollout(int Fd, std::map<int, Connection> &CMap,
                               const int &listen_sock,
                               std::vector<pollfd> &newFdBatch) {
  logging::log2(logging::Debug, "POLLOUT: fd ", Fd);
  const std::map<int, Connection>::iterator itC = CMap.find(Fd);
  if (itC != CMap.end()) {

    logging::log2(logging::Debug, "Ready to send to ", Fd);
    //send(Fd, "We got your request", 1024, MSG_DONTWAIT);
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
