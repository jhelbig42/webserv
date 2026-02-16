// TODO: add 42 header

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

// create_hints() is a helper function for get_server_info,
// which creates a "hints" addrinfo struct. "hints" contains the address
// info and I/O settings that we want to specify ourselves, when
// building our server's addrinfo.
//
// TODO: Check that we are allowed to use memset() & see if there's
// a more C++ way to initalize all fields to empty / 0.
//
// RETURNS: struct addrinfo with specified fields

static struct addrinfo create_hints(void) {

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints); // init struct to empty;
  hints.ai_family = AF_UNSPEC;     // allows either IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // for TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // autofill my IP
  return (hints);
}

// get_addr_info_str() is only for logging purposes.
// The function builds a string from the contents of an addrinfo struct
// and an additional msg, which I've used to add a label for this struct.
//
// NOTE: This function should not be used with uninitialized addrinfo structs,
// in which the values might be garbage. Currently, it is only used by
// get_server_info(), which assures initialization.
//
// Currently static due to one-time use. In the future, may be useful for
// logging client addrinfo, in which case it would need to be added back
// to the header, and could be moved to a different (debug / helper?) .cpp
//
// RETURNS: a string with all info to be printed

static std::string addrinfo_to_str(struct addrinfo *info, std::string msg) {
  std::ostringstream oss;
  oss << "\n\t" << msg << "\n"
      << "\tai_flags = " << info->ai_flags << "\n"
      << "\tai_family = " << info->ai_family << "\n"
      << "\tai_socktype = " << info->ai_socktype << "\n"
      << "\tai_protocol = " << info->ai_protocol << "\n"
      << "\tai_addrlen = " << info->ai_addrlen << "\n";
  return (oss.str());
}

// get_server_info() creates an addrinfo struct (standard, from <sys/socket.h>)
// with information about the server's own address & our chosen I/O settings.
// We need this struct to find a usable socket for listening.
//
// On error, resulting from library function call failure, an exception is
// thrown. Otherwise,
//
// RETURNS: ptr to an allocated addrinfo struct.
// Allocation is not our choice, but the result of system call getaddrinfo().
// The addrinfo MUST later be freed by system call freeaddrinfo()... easy.

struct addrinfo *networking::get_server_info(void) {

  struct addrinfo hints = create_hints();
  struct addrinfo *info;

  int ret = getaddrinfo(NULL, PORT, &hints, &info); // NULL = localhost
  if (ret != 0) {
    std::string msg(gai_strerror(ret));
    throw std::runtime_error("getaddrinfo: " + msg);
  } else {
    logging::log(logging::Debug, "addrinfo server_info created");
    logging::log(logging::Debug, addrinfo_to_str(info, "server_info:"));
  }
  return (info);
}
