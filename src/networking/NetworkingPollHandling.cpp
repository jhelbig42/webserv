/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NetworkingPollHandling.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:55:29 by hallison          #+#    #+#             */
/*   Updated: 2026/03/06 10:56:28 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Logging.hpp"
#include "Networking.hpp"
#include "NetworkingDefines.hpp" // can be removed?
#include <map>
#include <poll.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

// These functions handle the results of poll(), which are indicated
// by flags contained within the "revents" field of a given connection's
// pollfd struct.

void networking::handlePollnval(int Fd, std::map<int, Connection> &CMap);
void networking::handlePollerr(int Fd, std::map<int, Connection> &CMap);
void networking::handlePollin(int Fd, std::map<int, Connection> &CMap,
                              const int &listen_sock,
                              std::vector<pollfd> &newFdBatch);
void networking::handlePollout(int Fd, std::map<int, Connection> &CMap);
void networking::handlePollrdhup(int Fd, std::map<int, Connection> &CMap);
void networking::handleTerminalCondition(const short Revents, const int Fd,
                                         std::map<int, Connection> &CMap);
void networking::handleServableCondition(const int ListenSock,
                                         const short Revents, const int Fd,
                                         std::map<int, Connection> &CMap,
                                         std::vector<pollfd> &newFdBatch);


// handleTerminalCondition() handles flags which indicate a hangup or
// error. This path is in the logic is separated so we can skip checks
// for POLLIN and POLLOUT. Terminal conditions are handled with additional
// helper functions, which log specific errors / debug messages and mark
// the connection for deletion.

void networking::handleTerminalCondition(const short Revents, const int Fd,
                                         std::map<int, Connection> &CMap) {
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

// handleServableCondition() handles POLLIN & POLLOUT
// through the use of additional helper functions

void networking::handleServableCondition(const int ListenSock,
                                         const short Revents, const int Fd,
                                         std::map<int, Connection> &CMap,
                                         std::vector<pollfd> &newFdBatch) {
  if (Revents & POLLIN) { // data to read | hang-up
    handlePollin(Fd, CMap, ListenSock, newFdBatch);
  }
  if (Revents & POLLOUT) {
    handlePollout(Fd, CMap);
  }
}

// handlePollnval() handles POLLNVAL:
// 		invalid request: fd not open.

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

// handlePollrdhup() handles POLLRDHUP:
// 		Stream  socket peer closed connection, or shut down writing half 
//		of  connection.   The  _GNU_SOURCE  feature test macro must be defined
//		(before  including  any header files) in order to obtain this definition.
//
// TODO : inclusion of _GNU_SOURCE gave compiler error due to already defined?
// We clearly don't need it becuase POLLRDHUP is recognized, but perhaps there
// should be some kind of ifndef? May be machine specific?

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

// handlePollerr() handles POLLERR:
// 		Error  condition. This bit is also set for a file descriptor
//		referring  to  the  write  end of a pipe when the read end has been closed.

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

// handlePollin() handles POLLIN:
// 		There is data to read.

void networking::handlePollin(int Fd, std::map<int, Connection> &CMap,
                              const int &ListenSock,
                              std::vector<pollfd> &newFdBatch) {
  logging::log2(logging::Debug, "POLLIN: fd ", Fd);
  if (Fd == ListenSock) { // listening socket got new connection
    ClientAddr candidate;
    if (acceptConnection(ListenSock, &candidate) != -1) {
      addConnectionToMap(candidate, CMap);
      const short events =
          POLLIN | POLLERR | POLLHUP | POLLPRI | POLLRDHUP; // POLLOUT will be added when ready to send
      const pollfd newFd = {candidate.clientSock, events, 0};
      newFdBatch.push_back(newFd);
    }
  } else {
    const std::map<int, Connection>::iterator itC = CMap.find(Fd);
    if (itC != CMap.end()) {
      (itC->second).addToConditions(SockRead);
    } else {
      logging::log(logging::Error, "process: Connection not found in map "
                                   "container (This should never happen)");
      // could be removed after thorough testing
    }
  }
}

// handlePollout() handles POLLOUT:
// 		Writing is now possible.

void networking::handlePollout(int Fd, std::map<int, Connection> &CMap) {
  logging::log2(logging::Debug, "POLLOUT: fd ", Fd);
  const std::map<int, Connection>::iterator itC = CMap.find(Fd);
  if (itC != CMap.end()) {
    logging::log2(logging::Debug, "Ready to send to ", Fd);
    (itC->second).addToConditions(SockWrite);
  } else {
    logging::log(logging::Error, "process: Connection not found in map "
                                 "container (This should never happen)");
    // could be removed after thorough testing
  }
}
