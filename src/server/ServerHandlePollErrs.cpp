#include "Connection.hpp"
#include "Logging.hpp"
#include "Server.hpp"
#include <map>
#include <poll.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

// ServerHandlePollErrs.cpp
//
// These functions handle revents which indicate that the connection
// should be deleted, include errors and client hangups.

///////////////////////////////////////////////////////////////////////////

// handlePollnval() handles POLLNVAL:
// 		invalid request: fd not open.

void Server::handlePollnval(int Fd, int Type) {
  logging::log2(logging::Error,
                "networking::process(): POLLNAL.\n\tpoll() tried to read "
                "invalid fd. fd = ",
                Fd);
  if (Type == IS_CLIENT) {
    logging::log2(logging::Error, Fd, " is a client socket");
    logging::log2(logging::Debug, Fd,
                  " scheduleForDemolition() in handlePollnval()");
    _fwdMap.at(Fd)->scheduleForDemolition();
    return;
  }
  if (Type == IS_FWD) {
    logging::log2(logging::Error, Fd, " is forward socket");
    return;
  }
  if (Type == IS_LISTENER) {
    logging::log2(logging::Error, Fd, " is listening socket");
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
    logging::log2(logging::Debug, Fd,
                  " scheduleForDemolition() in handlePollhup()");
    _clientMap.at(Fd).scheduleForDemolition();
  } else if (Type == IS_FWD) {
    logging::log3(logging::Debug,
                  "networking::handlePollhup(): got POLLHUP from fwd sock ", Fd,
                  ". Indicates at least one of the socket pairs closed");
    return;
  }
  if (Type == IS_LISTENER) {
    logging::log2(logging::Error, Fd,
                  " is listening socket. This is truly bizarre.");
    return;
  }
}

// handlePollerr() handles POLLERR:
// 		Error  condition. This bit is also set for a file descriptor
//		referring  to  the  write  end of a pipe when the read end has been
//		closed.

void Server::handlePollerr(int Fd, int Type) {
  const std::ostringstream msg;
  logging::log2(logging::Info,
                "networking::process(): POLLERR \n\tclient/cgi may have "
                "disconnected abruptly using RST (forcible reset), or socket "
                "is broken.\n fd = ",
                Fd);
  if (Type == IS_CLIENT) {
    logging::log2(logging::Info, Fd, " is a client socket");
    // logging::log2(logging::Debug, Fd, " scheduleForDemolition() in
    // handlePollerr()");
    _clientMap.at(Fd).scheduleForDemolition();
    return;
  }
  if (Type == IS_FWD) {
    logging::log2(logging::Info, Fd, " is forward socket");
    int error;
    socklen_t len = sizeof(int);
    getsockopt(Fd, SOL_SOCKET, SO_ERROR, &error, &len);
    logging::log2(logging::Info, "POLLERR error = ", strerror(error));
    return;
  }
  if (Type == IS_LISTENER) {
    logging::log2(logging::Info, Fd,
                  " is listening socket. This is truly bizarre.");
    return;
  }
}
