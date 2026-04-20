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

void Server::handleCondition(struct pollfd &polled) {
  /*
  if (reventsAreTerminal(polled.revents)) {
    handleTerminalCondition(polled);
  } else {
    handleServableCondition(polled);
  }
  */
  int type = getSocketType(polled.fd);
  if (type == -1) {
    logging::log3(logging::Error, "handleCondition(): fd ", polled.fd,
                  "is not found in _clientMap or _fwdMap");
    return;
  }
  if (polled.revents & POLLNVAL) {
    handlePollnval(polled.fd, type);
    return;
  }
  if (polled.revents & POLLERR) {
    handlePollerr(polled.fd, type);
    return;
  }
  if (polled.revents & POLLHUP) {
    handlePollhup(polled.fd, type);
    return;
  }
  if ((polled.revents & POLLIN) || polled.revents & POLLPRI) {
    handlePollin(polled.fd, type);
  }
  if (polled.revents & POLLOUT) {
    handlePollout(polled.fd, type);
  }
  if (polled.revents & POLLRDHUP) {
    handlePollrdhup(polled.fd, type);
  }
}

/**
 *	\brief reventsAreTerminal() checks if poll returned error or
 *	hangup in revents, which means the connection should be terminated
 *
 *	\param	revents, the field in pollfd filled by poll()
 *	\return true if conditions are found, otherwise false
 */

/*
bool Server::reventsAreTerminal(int revents) {
 if ((revents & POLLNVAL) || (revents & POLLERR) || (revents & POLLHUP)) {
   return true;
 }
 return false;
}
 */

/**
 *	\brief handleTerminalCondition() handles revents flags
 *	which indicate hangup or error. In this case, the Connection
 *	is scheduled for deletion.
 *
 *	\param Polled	pollfd belonging to a client connection
 */

/*
void Server::handleTerminalCondition(struct pollfd &Polled) {
 if (Polled.revents & POLLNVAL) {
   handlePollnval(Polled.fd);
   return;
 }
 if (Polled.revents & POLLERR) {
   handlePollerr(Polled.fd);
   return;
 }
 if (Polled.revents & POLLHUP) {
   handlePollhup(Polled.fd);
   return;
 }
}
*/

/**
 *	\brief handleServableCondition() handles POLLIN & POLLOUT
 *	as well as POLLPRI (high priority POLLIN)
 *	& POLLRDHUP (client has finished writing but may still read)
 *
 */

/*
void Server::handleServableCondition(struct pollfd &Polled) {

 // POLLPRI = high priority data to read.
 if (Polled.revents & POLLIN || Polled.revents & POLLPRI) {
   handlePollin(Polled.fd);
 }
 if (Polled.revents & POLLOUT) {
   handlePollout(Polled.fd);
 }
}
*/
