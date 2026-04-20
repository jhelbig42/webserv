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

// ServerHandlePollErrs.cpp
//
// These functions handle revents which indicate that the connection
// should be deleted, include errors and client hangups.

///////////////////////////////////////////////////////////////////////////


/*
*	markClientDeletion() finds the Connection that corresponds to Fd
*	and calls scheduleForDemolition (a function belonging to Connection class)
*	to mark it for disconnection. If no Connection corresponding to Fd
*	is found, an error is logged.
*
*	This function is currently not in use because we are switching to a method
* 	of revents handling that predetermines whether Fd is in clientMap or fwdMap.
*
*	// TODO delete this function after thorough testing with new method
*/

/*
void Server::markClientDeletion(int Fd) {
  try {
    _clientMap.at(Fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log3(logging::Error, "markClientDeletion(): ", Fd,
                  " could not be marked for deletion because there is "
                  "no Connection in _clientMap with this fd."
                  "(This should never happen).");
  }
}
*/

/*
*	markFwdDeletion() finds the Connection that corresponds to a foward socket Fd
*	and calls scheduleForDemolition (a function belonging to Connection class)
*	to mark it for disconnection. If no Connection corresponding to Fd
*	is found, an error is logged.
*
*	This function is currently not in use. I had thought it would come in handy
*	but deleting Connections due to fwd socket error may not be something we ever
*	want to do.
*
*	// TODO delete this function after more testing
*/

/*
void Server::markFwdDeletion(int Fd) {

  try {
    _fwdMap.at(Fd)->scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log3(logging::Error, "markFwdDeletion(): ", Fd,
                  " could not be marked for deletion because there is "
                  "no Connection in _fwdMap with this fd."
                  "(This should never happen).");
  }
}
*/

// handlePollnval() handles POLLNVAL:
// 		invalid request: fd not open.

void Server::handlePollnval(int Fd, int Type) {
  logging::log2(logging::Error,
                "networking::process(): POLLNAL.\n\tpoll() tried to read "
                "invalid fd. fd = ",
                Fd);
  if (Type == IS_CLIENT) {
    logging::log2(logging::Error, Fd, " is a client socket");
    //markClientDeletion(Fd);
	_fwdMap.at(Fd)->scheduleForDemolition();
	return;
  }
  if (Type == IS_FWD) {
    logging::log2(logging::Error, Fd, " is forward socket");
    // TODO how to handle this
    // markFwdDeletion(Fd);
    return;
  }
}

// handlePollrdhup() handles POLLRDHUP:
// 		Stream  socket peer closed connection, or shut down writing half
//		of  connection.   The  _GNU_SOURCE  feature test macro must be defined
//		(before  including  any header files) in order to obtain this definition.
//
// TODO : inclusion of _GNU_SOURCE gave compiler error due to already defined?
// I've tried including at the top of main, and compiler still complains

void Server::handlePollhup(int Fd, int Type) {
  logging::log2(logging::Debug,
                "networking::process(): POLLHUP.\n\t client hung up."
                " fd = ",
                Fd);
  if (Type == IS_CLIENT) {
    _clientMap.at(Fd).scheduleForDemolition();
  } else if (Type == IS_FWD) {
    logging::log3(logging::Debug,
                  "networking::handlePollhup(): got POLLHUP from fwd sock ", Fd,
                  ". This is not necessarily an error but interesting to "
                  "observe when this happens. We may want to implement removal "
                  "of the fwd sock at this point.");
    // TODO downgraed this print before submission
    // TODO remove socket from fwdMap? // use markFwdDeletion?
  }
}

// handlePollerr() handles POLLERR:
// 		Error  condition. This bit is also set for a file descriptor
//		referring  to  the  write  end of a pipe when the read end has been
//		closed.

void Server::handlePollerr(int Fd, int Type) {
  const std::ostringstream msg;
  logging::log2(logging::Error,
                "networking::process(): POLLERR \n\tclient may have "
                "disconnected abruptly using RST, or socket is broken.\n fd = ",
                Fd); // downgrade to Warning?
  if (Type == IS_CLIENT) {
    logging::log2(logging::Error, Fd, " is a client socket");
    //markClientDeletion(Fd);
    _clientMap.at(Fd).scheduleForDemolition();
    return;
  }
  if (Type == IS_FWD) {
    logging::log2(logging::Error, Fd, " is forward socket");
    // a series of returns
    return;
  }
}
