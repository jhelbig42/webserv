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

void Server::markClientDeletion(int Fd){
  try {
    _clientMap.at(Fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log3(logging::Error,
                 "markClientDeletion(): ", Fd, 
				 " could not be marked for deletion because there is "
                 "no Connection in _clientMap with this fd."
				 "(This should never happen).");
  }
}

void Server::markFwdDeletion(int Fd){
  try {
    _fwdMap.at(Fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log3(logging::Error,
                 "markFwdDeletion(): ", Fd, 
				 " could not be marked for deletion because there is "
                 "no Connection in _fwdMap with this fd."
				 "(This should never happen).");
  }
}


// handlePollnval() handles POLLNVAL:
// 		invalid request: fd not open.

void Server::handlePollnval(int Fd) {
  logging::log2(logging::Error,
                "networking::process(): POLLNAL.\n\tpoll() tried to read "
                "invalid fd. fd = ",
                Fd);
	if (socketIsClient(Fd)){
		logging::log2(logging::Error, Fd, " is a client socket");
		markClientDeletion(Fd);
	}
	if (socketIsFwd(Fd)){
		logging::log2(logging::Error, Fd, " is forward socket");
		// TODO how to handle this
		//markFwdDeletion(Fd);
	}
}

// handlePollrdhup() handles POLLRDHUP:
// 		Stream  socket peer closed connection, or shut down writing half
//		of  connection.   The  _GNU_SOURCE  feature test macro must be defined
//		(before  including  any header files) in order to obtain this definition.
//
// TODO : inclusion of _GNU_SOURCE gave compiler error due to already defined?
// I've tried including at the top of main, and compiler still complains

void Server::handlePollhup(int Fd) {
  logging::log2(logging::Debug,
                "networking::process(): POLLHUP.\n\t client hung up."
                " fd = ",
                Fd);
  try {
    _clientMap.at(Fd).scheduleForDemolition();
  } catch (const std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}

// handlePollerr() handles POLLERR:
// 		Error  condition. This bit is also set for a file descriptor
//		referring  to  the  write  end of a pipe when the read end has been
//		closed.

void Server::handlePollerr(int Fd) {
  const std::ostringstream msg;
  logging::log2(logging::Error,
                "networking::process(): POLLERR \n\tclient may have "
                "disconnected abruptly using RST, or socket is broken.\n fd = ",
                Fd); // downgrade to Warning?
  try {
    _clientMap.at(Fd).scheduleForDemolition();
  } catch (std::out_of_range &e) {
    logging::log(logging::Error,
                 "Connection could not be marked for deletion because there is "
                 "no Connection with this fd.");
  }
}
