/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:19:03 by hallison          #+#    #+#             */
/*   Updated: 2026/02/24 12:24:31 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conditions.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "Networking.hpp" // needed for sockaddr_storage
// sockaddr_storage could possibly be defined here
// or somewhere else
#include "NetworkingDefines.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <cerrno> // for errno
#include <cstring> // for strerror
#include <cstddef>
#include <sstream> // for ostreamstring
#include <stddef.h>
#include <stdio.h> // for puts
#include <string.h> // for memcpy, memset
#include <sys/socket.h> // for socklen_t, recv

#define BYTES_PER_CHUNK 256

// Construct & Destruct

Connection::Connection(const int Sock, const sockaddr_storage &Addr,
                       const socklen_t Addr_size)
    : _delete(false), _sock(Sock), _addrSize(sizeof _addr) {

  memset(&_info, 0, sizeof _info); // unneccessary? delete?
  memcpy(&_addr, &Addr, Addr_size);
  memset(&_readBuf, 0, MAX_REQUEST);
  logging::log(logging::Debug, "Connection created");
}

// Getters

Connection::~Connection(void) {
}

int Connection::getSock(void) const {
  return (_sock);
}

Conditions Connection::getConditions(void) const {
  if (_req.isFullyParsed())
		return _res.getConditions();
	return _req.getConditions();
}

// Setters

void Connection::scheduleForDemolition(void) {
  _delete = false;
}

// Send & Receive

void Connection::readData(void) {

  logging::log(logging::Debug, "read_data()");
  const ssize_t bytesRead = recv(_sock, &_readBuf, MAX_REQUEST, 0);

  if (bytesRead == MAX_REQUEST) {
    logging::log(logging::Info, "read_data(): bytes_read == MAX REQUEST");
    // May happen frequently, will be handled in chunks
    // Logging for debug purposes as we build.
  }
  if (bytesRead == 0) {
    logging::log(logging::Warning, "read_data(): bytes_read == 0");
    _delete = true; // important to coordinate with Julia / parsing
    logging::log(logging::Warning, "client appears to have hung up.");
    return;
  }
  if (bytesRead < 0) {
    std::ostringstream msg;
    msg << "recv: " << std::strerror(errno);
    logging::log(logging::Warning, msg.str());
    return;
  }
  _readBuf[bytesRead - 1] = '\0';
  //	puts(_read_buf);
  //_req.init("GET /home/hallison/webserv/.gitignore HTTP/1.0");
  _req.init(_readBuf);
  _res.init(_req);
  int dummy = -1;
  while (!_res.process(_sock, dummy, BYTES_PER_CHUNK))
    ;
  logging::log(logging::Debug, "read_buf = ");
  logging::log(logging::Debug, _readBuf);
}

bool Connection::serve(const size_t Bytes) {
  if (!_req.isFullyParsed()) {
		if (_conditionsFulfilled & _req.getConditions())
			_req.process(_sock, Bytes);
		if (_req.isFullyParsed())
			_res.init(_req);
    return false;
  }
	
  if (_conditionsFulfilled & _res.getConditions())
    return _res.process(_sock, _sockForward, Bytes);
  return false;
}

