/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:19:03 by hallison          #+#    #+#             */
/*   Updated: 2026/02/25 15:49:29 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Conditions.hpp"
#include "Logging.hpp"
#include "NetworkingDefines.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <cerrno> // for errno
#include <cstddef>
#include <cstring> // for strerror
#include <sstream> // for ostreamstring
#include <stddef.h>
#include <stdio.h>      // for puts
#include <string.h>     // for memcpy, memset
#include <sys/socket.h> // for socklen_t, recv
#include <sys/types.h>  // for ssize_t

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
  if (_req.getState() == COMPLETE)
		return _res.getConditions();
	return _req.getConditions();
}

// Setters

void Connection::scheduleForDemolition(void) {
  _delete = true;
}

// Send & Receive

/*
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
  logging::log2(logging::Debug, "read_buf = ", _readBuf);

  //_req.init(_readBuf);
  //_res.init(_req);
  //int dummy = -1;
  //while (!_res.process(_sock, dummy, BYTES_PER_CHUNK))
  //  ;
}*/

// Send & Receive
//processData is a smaller type of serve() until conditions are fully implemented
void Connection::processData(void) {
	if(_req.getState() != COMPLETE)
		_req.process(_sock);
	
	if (_req.ClientHungUp){
		scheduleForDemolition(); // this does not result in deletion
							// of Connection yet --> Hilary?
		return ;
	};
		 
	//parsing from buffer into Request
	//when fully parsed init response 
	if(_req.getState() == COMPLETE)
	{
		_res.init(_req);
		int dummy = -1;
		while (!_res.process(_sock, dummy, BYTES_PER_CHUNK))
  			 ;	}
	if (_req.ClientHungUp){
		scheduleForDemolition(); // this does not result in deletion
							// of Connection yet --> Hilary?
		return ;
	};
}

/*
bool Connection::serve(const size_t Bytes) {
  //handle Request until fully parsed
	if (_req.getState() == COMPLETE) {
		if (_conditionsFulfilled & _req.getConditions())
			_req.process(_sock);
		if (_req.getState() == COMPLETE)
			_res.init(_req);
    return false;
  }
  //switch to Response	
  if (_conditionsFulfilled & _res.getConditions())
    return _res.process(_sock, _sockForward, Bytes);
  return false;
}
*/

bool Connection::serve(const size_t Bytes) {
  //handle Request until fully parsed
	logging::log2(logging::Debug, "_state = ", _req.getState());
	if (_req.getState() == COMPLETE) {
		if (_conditionsFulfilled & _req.getConditions())
			_req.process(_sock);
		if (_req.getState() == COMPLETE)
			_res.init(_req);
    return false;
  }
  //switch to Response	
  if (_conditionsFulfilled & _res.getConditions())
    return _res.process(_sock, _sockForward, Bytes);
  return false;
}

void Connection::addToConditions(Conditions Condition) {
  _conditionsFulfilled = _conditionsFulfilled | Condition;
}

void Connection::resetConditions(void) {
  _conditionsFulfilled = 0;
}
