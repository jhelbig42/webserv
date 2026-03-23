#include "Logging.hpp"
#include "Server.hpp"
#include "Website.hpp"
#include <cerrno>  // for errno
#include <cstring> // for strerror

void Server::handleBindFailure(const struct addrinfo *Info, const Listen &Interface, const int Error) {
  std::ostringstream msg;
  msg << "bind: ";
      switch (Error) {
      case EACCES:
        msg << "EACCESS: " << std::strerror(Error) << "\n" << interfaceInfoToStr(Interface)
            << "Use a port number above 1024, or run webserv with root privileges.\n";
        break;
      case EADDRNOTAVAIL:
        msg << "EADDRNOTAVAIL: " << std::strerror(Error) << "\n" << interfaceInfoToStr(Interface)
        << "Check that the IP address in the config file is correct for this machine\n"
        << "Use 127.0.0.1 for local host, or find the machine's IP by running 'ip addr' or 'ifconfig'\n";
        break;
	msg << std::strerror(Error) << "\n" << interfaceInfoToStr(Interface)
		<< "Bind failure. Check that config file contents for invalid interfaces, and check that there is no other instance of webserv already running with the same config.\n"
		<< "If bind continues to fail, check elsewhere for occupied interfaces, using ss or netstat & the flags -tulnp";
      // TODO handle additional errno cases
      };
  (void) Info;
  logging::log(logging::Error, msg.str());
}