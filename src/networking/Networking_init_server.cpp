/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking_init_server.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 14:06:35 by hallison          #+#    #+#             */
/*   Updated: 2026/02/24 14:11:56 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logging.hpp"
#include "Networking.hpp"
#include "NetworkingDefines.hpp"
#include <cstddef> // for NULL
#include <netdb.h>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <string.h>

struct addrinfo *networking::getServerInfo(void);
static struct addrinfo createHints(void);
static std::string addrinfoToStr(const struct addrinfo *info,
                                   const std::string &msg);

// getServerInfo() creates an addrinfo struct (standard, from <sys/socket.h>)
// with information about the server's own address & our chosen I/O settings.
// We need this struct to find a usable socket for listening.
//
// On error, resulting from library function call failure, an exception is
// thrown. Otherwise,
//
// RETURNS: ptr to an allocated addrinfo struct.
// Allocation is not our choice, but the result of system call getaddrinfo().
// The addrinfo MUST later be freed by system call freeaddrinfo()... easy.

struct addrinfo *networking::getServerInfo(void) {

  const struct addrinfo hints = createHints();
  struct addrinfo *info;

  const int ret = getaddrinfo(NULL, PORT, &hints, &info); // NULL = localhost
  if (ret != 0) {
    const std::string msg(gai_strerror(ret));
    throw std::runtime_error("getaddrinfo: " + msg);
  }
  logging::log(logging::Debug, "addrinfo server_info created");
  logging::log(logging::Debug, addrinfoToStr(info, "server_info:"));
  return (info);
}

// createHints() is a helper function for getServerInfo,
// which creates a "hints" addrinfo struct. "hints" contains the address
// info and I/O settings that we want to specify ourselves, when
// building our server's addrinfo.
//
// TODO: Check that we are allowed to use memset() & see if there's
// a more C++ way to initalize all fields to empty / 0.
//
// RETURNS: struct addrinfo with specified fields

static struct addrinfo createHints(void) {

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
// getServerInfo(), which assures initialization.
//
// Currently static due to one-time use. In the future, may be useful for
// logging client addrinfo, in which case it would need to be added back
// to the header, and could be moved to a different (debug / helper?) .cpp
//
// RETURNS: a string with all info to be printed

static std::string addrinfoToStr(const struct addrinfo *info,
                                   const std::string &msg) {
  std::ostringstream oss;
  oss << "\n\t" << msg << "\n"
      << "\tai_flags = " << info->ai_flags << "\n"
      << "\tai_family = " << info->ai_family << "\n"
      << "\tai_socktype = " << info->ai_socktype << "\n"
      << "\tai_protocol = " << info->ai_protocol << "\n"
      << "\tai_addrlen = " << info->ai_addrlen << "\n";
  return (oss.str());
}
