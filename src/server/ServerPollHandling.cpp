#include "Connection.hpp"
#include "Logging.hpp"
#include "Server.hpp"
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

// handleTerminalCondition() handles flags which indicate a hangup or
// error. This path is in the logic is separated so we can skip checks
// for POLLIN and POLLOUT. Terminal conditions are handled with additional
// helper functions, which log specific errors / debug messages and mark
// the connection for deletion.

void Server::handleTerminalCondition(struct pollfd &Polled) {
  if (Polled.revents & POLLNVAL) {
    handlePollnval(Polled.fd);
    exit(1);
  }
  if (Polled.revents & POLLERR) {
    handlePollerr(Polled.fd);
    exit(1);
  }
  if (Polled.revents & POLLRDHUP) {
    handlePollrdhup(Polled.fd);
  }
  if (Polled.revents & POLLHUP) {
    logging::log2(logging::Debug, "Hangup from fd ", Polled.fd);
    exit(1);
    // TODO
  }
  if (Polled.revents & POLLPRI) {
    logging::log2(logging::Debug, "POLLPRI from fd ", Polled.fd);
    exit(1);
    // TODO
  }
}

// handleServableCondition() handles POLLIN & POLLOUT
// through the use of additional helper functions

void Server::handleServableCondition(struct pollfd &Polled) {
  if (Polled.revents & POLLIN) { // data to read | hang-up
    handlePollin(Polled.fd);
  }
  if (Polled.revents & POLLOUT) {
    handlePollout(Polled.fd);
  }
}

// handlePollnval() handles POLLNVAL:
// 		invalid request: fd not open.

void Server::handlePollnval(int Fd) {
  logging::log2(logging::Error,
                "networking::process(): POLLNAL.\n\tpoll() tried to read "
                "invalid fd. fd = ",
                Fd);
  try {
    clientMap.at(Fd).scheduleForDemolition();
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

void Server::handlePollrdhup(int Fd) {
  logging::log2(logging::Debug,
                "networking::process(): POLLRDHUP.\n\t client hung up."
                " fd = ",
                Fd);
  try {
    clientMap.at(Fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

// handlePollerr() handles POLLERR:
// 		Error  condition. This bit is also set for a file descriptor
//		referring  to  the  write  end of a pipe when the read end has been
// closed.

void Server::handlePollerr(int Fd) {
  const std::ostringstream msg;
  logging::log2(logging::Error,
                "networking::process(): POLLERR \n\tclient may have "
                "disconnected abruptly using RST, or socket is broken.\n fd = ",
                Fd); // downgrade to Warning?
  try {
    clientMap.at(Fd).scheduleForDemolition();
  } catch (std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

// handlePollin() handles POLLIN:
// 		There is data to read.

void Server::handlePollin(int Fd) {
  logging::log2(logging::Debug, "POLLIN: fd ", Fd);
  if (listenMap.find(Fd) !=
      listenMap.end()) { // listening socket got new connection
                         // TODO make this a getter if connectionIsListener
    ClientAddr candidate;
    if (acceptConnection(Fd, &candidate) != -1) {
      addConnectionToMap(Fd, candidate);
      const short events =
          POLLIN | POLLERR | POLLHUP | POLLPRI | POLLRDHUP | POLLOUT;
      const pollfd newFd = {candidate.clientSock, events, 0};
      newFdBatch.push_back(newFd);
    }
  } else {
    const std::map<int, Connection>::iterator itC = clientMap.find(Fd);
    if (itC != clientMap.end()) {
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

void Server::handlePollout(int Fd) {
  //  logging::log2(logging::Debug, "POLLOUT: fd ", Fd);
  const std::map<int, Connection>::iterator itC = clientMap.find(Fd);
  if (itC != clientMap.end()) {
    // logging::log2(logging::Debug, "Ready to send to ", Fd);
    (itC->second).addToConditions(SockWrite);
  } else {
    logging::log(logging::Error, "process: Connection not found in map "
                                 "container (This should never happen)");
    // could be removed after thorough testing
  }
}
