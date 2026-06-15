#include "Server.hpp"
#include <cerrno>  // for errno
#include <cstring> // for strerror

//////////////////////////////////////////////////////////////////////////

// #include <fcntl.h> // for demonstration purposes only
// fcntl() is not an allowed function, but can be used to demonstrate
// that client sockets are blockinb by default. Uncomment printFcntlFlags()

///////////////////////////////////////////////////////////////////////////

/**
 * \brief Converts addrinfo struct to a string for logging purposes.
 *
 * Currently, it is only used by getServerInfo().
 *
 * \param    Info pointer to an addrinfo struct
 * \param    Msg a string to be included in the output, i.e. a label / title
 *
 * \return a string with all info to be printed
 */

std::string Server::addrinfoToStr(const struct addrinfo *Info,
                                  const std::string &Msg) {
  std::ostringstream oss;
  oss << "\n\t" << Msg << "\n"
      << "\tai_flags = " << Info->ai_flags << "\n"
      << "\tai_family = " << Info->ai_family << "\n"
      << "\tai_socktype = " << Info->ai_socktype << "\n"
      << "\tai_protocol = " << Info->ai_protocol << "\n"
      << "\tai_addrlen = " << Info->ai_addrlen << "\n";
  return (oss.str());
}

std::string Server::interfaceInfoToStr(const Listen &Interface) {
  std::ostringstream msg;
  msg << "\n"
      << "IP: " << Interface.ip << "\n"
      << "Port: " << Interface.port << "\n\n";
  return (msg.str());
}

std::string Server::getTypeString(int Type) {
  if (Type == IS_CLIENT) {
    return ("client");
  } else if (Type == IS_FWD) {
    return ("forward");
  } else if (Type == IS_LISTENER) {
    return ("listener");
  } else {
    return ("nonsense");
  }
}

std::string Server::getConditionsFulfilledString(int CFulfilled) {
  std::ostringstream fulfilled;
  if (CFulfilled & SockRead) {
    fulfilled << "SockRead ";
  }
  if (CFulfilled & SockWrite) {
    fulfilled << "SockWrite ";
  }
  if (CFulfilled & FSockRead) {
    fulfilled << "FSockRead ";
  }
  if (CFulfilled & FSockWrite) {
    fulfilled << "FSockWrite ";
  }
  return (fulfilled.str());
}

std::string Server::getConditionsWantedString(int CWanted) {
  std::ostringstream wanted;
  if (CWanted & SockRead) {
    wanted << "SockRead ";
  }
  if (CWanted & SockWrite) {
    wanted << "SockWrite ";
  }
  if (CWanted & FSockRead) {
    wanted << "FSockRead ";
  }
  if (CWanted & FSockWrite) {
    wanted << "FSockWrite ";
  }
  return (wanted.str());
}

std::string Server::getEventsString(short Events) {
  std::ostringstream msg;
  if (Events & POLLNVAL) {
    msg << "POLLNVAL ";
  }
  if (Events & POLLERR) {
    msg << "POLLERR ";
  }
  if (Events & POLLIN) {
    msg << "POLLIN ";
  }
  if (Events & POLLOUT) {
    msg << "POLLOUT ";
  }
  if (Events & POLLRDHUP) {
    msg << "POLLRDHUP ";
  }
  if (Events & POLLHUP) {
    msg << "POLLHUP ";
  }
  return (msg.str());
}

std::string Server::getReventsString(short Revents) {
  std::ostringstream msg;
  if (Revents & POLLNVAL) {
    msg << "POLLNVAL ";
  }
  if (Revents & POLLERR) {
    msg << "POLLERR ";
  }
  if (Revents & POLLIN) {
    msg << "POLLIN ";
  }
  if (Revents & POLLOUT) {
    msg << "POLLOUT ";
  }
  if (Revents & POLLRDHUP) {
    msg << "POLLRDHUP ";
  }
  if (Revents & POLLHUP) {
    msg << "POLLHUP ";
  }
  return (msg.str());
}

std::string Server::getFdInfoString(pollfd &it, int Fd, int Type) {
  std::ostringstream msg;
  msg << "\n"
      << "\tfd: " << Fd << "\n"
      << "\ttype: " << getTypeString(Type) << "\n";
  if (Type == IS_CLIENT) {
    msg << "\t_sockForward = " << _clientMap.at(Fd).getSockForward() << "\n"
        << "\t_cgiFinished = " << _clientMap.at(Fd).getCgiFinishedStatus()
        << "\n"
        << "\t_conditionsWanted: "
        << getConditionsWantedString(_clientMap.at(Fd).getConditionsWanted())
        << "\n"
        << "\t_conditionsFulfilled: "
        << getConditionsFulfilledString(
               _clientMap.at(Fd).getConditionsFulfilled())
        << "\n";
  }
  if (Type == IS_FWD) {
    msg << "\tbelongs to client: " << _fwdMap.at(Fd)->getSock() << "\n"
        << "\t_conditionsWanted: "
        << getConditionsWantedString(_fwdMap.at(Fd)->getConditionsWanted())
        << "\n"
        << "\t_conditionsFulfilled: "
        << getConditionsFulfilledString(
               _fwdMap.at(Fd)->getConditionsFulfilled())
        << "\n";
  }
  msg << "\tevents: " << getReventsString(it.events) << "\n"
<<<<<<< HEAD
      << "\trevents: " << getReventsString(it.revents) << "\n";
||||||| parent of 89d8821 (fix(clang-format all))
  << "\trevents: " << getReventsString(it.revents) << "\n"
  << "\n";
=======
      << "\trevents: " << getReventsString(it.revents) << "\n"
      << "\n";
>>>>>>> 89d8821 (fix(clang-format all))
  return (msg.str());
}

/*
void Server::printFcntlFlags(const int Sock) {
  const int flags = fcntl(Sock, F_GETFL);
  logging::log3(logging::Debug, Sock, " fcntl flags = ", flags);
  if (flags & O_NONBLOCK) {
    logging::log2(logging::Debug, Sock, " is NON-BLOCKING");
  } else {
    logging::log2(logging::Debug, Sock, " is BLOCKING");
  }
}
*/
