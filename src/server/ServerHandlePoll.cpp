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

// ServerHandlePoll.cpp
//
// These functions begin the handling the poll() results,
// which are indicated by flags contained within the "revents"
// field of a given connection's pollfd struct.


/////////////////////////////////////////////////////////////////////////

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

