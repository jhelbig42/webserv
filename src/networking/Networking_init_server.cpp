#include "Networking.hpp"
#include "NetworkingDefines.hpp"


struct addrinfo networking::create_hints(void) {

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints); // init struct to empty;
  hints.ai_family = AF_UNSPEC;     // allows either IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // for TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // autofill my IP
  return (hints);
}

std::string networking::get_addrinfo_str(struct addrinfo *info,
                                         std::string msg) {
  std::ostringstream oss;
  oss << "\n\t" << msg << "\n"
      << "\tai_flags = " << info->ai_flags << "\n"
      << "\tai_family = " << info->ai_family << "\n"
      << "\tai_socktype = " << info->ai_socktype << "\n"
      << "\tai_protocol = " << info->ai_protocol << "\n"
      << "\tai_addrlen = " << info->ai_addrlen << "\n";
  return (oss.str());
}

struct addrinfo *networking::get_server_info(void) {

  struct addrinfo hints = create_hints();
  struct addrinfo *info;

  int ret = getaddrinfo(NULL, PORT, &hints, &info); // NULL = localhost
  if (ret != 0) {
    std::string msg(gai_strerror(ret));
    throw std::runtime_error("getaddrinfo: " + msg);
  } else {
    logging::log(logging::Debug, "addrinfo server_info created");
    logging::log(logging::Debug, get_addrinfo_str(info, "server_info:"));
  }
  return (info);
}
