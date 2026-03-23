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

/** 
*	\brief reventsAreTerminal() checks if poll returned error or
*	hangup in revents, which means the connection should be terminated
*
*	\param	revents, the field in pollfd filled by poll()
*	\return true if conditions are found, otherwise false
*/

bool Server::reventsAreTerminal(int revents) {
  if ((revents & POLLNVAL) || (revents & POLLERR) || (revents & POLLHUP) ||
      (revents & POLLPRI)) {
    return true;
  }
  return false;
}

/**
*	\brief handleTerminalCondition() handles revents flags
*	which indicate hangup or error. In this case, the Connection
*	is scheduled for deletion.
*
*	\param Polled	pollfd belonging to a client connection
*/

void Server::handleTerminalCondition(struct pollfd &Polled) {
  if (Polled.revents & POLLNVAL) {
    handlePollnval(Polled.fd);
    exit(1);
  }
  if (Polled.revents & POLLERR) {
    handlePollerr(Polled.fd);
    exit(1);
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

  /*
  if (Polled.revents & POLLRDHUP) {
  	// TODO
  }
  */
  if (Polled.revents & POLLIN) {
    handlePollin(Polled.fd);
  }
  if (Polled.revents & POLLOUT) {
    handlePollout(Polled.fd);
  }
}
