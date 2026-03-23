#include "Logging.hpp"
#include "Server.hpp"
#include <poll.h>
#include <sstream>
#include <stdexcept>

// handlePollErrs.cpp is for the handling of poll revents which result
// in connection removal, including both errors and hangups.


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

