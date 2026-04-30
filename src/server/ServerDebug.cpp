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
