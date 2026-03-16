#include "Server.hpp"
#include <cstring>

// get_addr_info_str() is only for logging purposes.
// The function builds a string from the contents of an addrinfo struct
// and an additional msg, which I've used to add a label for this struct.
//
// NOTE: This function should not be used with uninitialized addrinfo structs,
// in which the values might be garbage. Currently, it is only used by
// getServerInfo(), which assures initialization.
//
// Currently static due to one-time use. In the future, may be useful for
// logging client addrinfo, in which case it would need to be added back
// to the header, and could be moved to a different (debug / helper?) .cpp
//
// RETURNS: a string with all info to be printed

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

void Server::handleBindFailure(const struct addrinfo *Info, const Listen &Interface, const int Error) {
  std::ostringstream msg;
  msg << "bind: " << std::strerror(Error) << "\n"
      << "Failed to bind to interface.\n"
      << "IP:" << Interface.ip << "\n"
      << "Port:" << Interface.port << "\n"
      << "Ensure that config file contains no duplicate interfaces, and check that there is no other instance of webserv already running with the same interface.\n"
      << "If bind continues to fail, check elsewhere for occupied interfaces, using ss or netstat & the flags -tulnp";
  //    << addrinfoToStr(Info, "Interface info:");
  // TODO better debug instructions, errno dependant & print more Info
  (void) Info;
  logging::log(logging::Error, msg.str());
}