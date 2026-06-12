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

void Server::handleCondition(struct pollfd &polled, int Type, time_t TimeNow) {

  if (polled.revents & POLLNVAL) {
    handlePollnval(polled.fd, Type);
    return;
  }
  if (polled.revents & POLLERR) {
    handlePollerr(polled.fd, Type);
    return;
  }
  if ((polled.revents & POLLIN) || polled.revents & POLLPRI) {
    handlePollin(polled.fd, Type);
    if (Type == IS_CLIENT) {
      _clientMap.at(polled.fd).setTimeLastActive(TimeNow);
    }
  }
  if (polled.revents & POLLOUT) {
    handlePollout(polled.fd, Type);
    if (Type == IS_CLIENT) {
      _clientMap.at(polled.fd).setTimeLastActive(TimeNow);
    }
  }
  if (polled.revents & POLLHUP) {
    handlePollhup(polled.fd, Type);
    return;
  }
}
