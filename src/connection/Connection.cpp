/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:19:03 by hallison          #+#    #+#             */
/*   Updated: 2026/02/24 14:34:49 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conditions.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "NetworkingDefines.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <cerrno> // for errno
#include <cstddef>
#include <cstring> // for strerror
#include <sstream> // for ostreamstring
#include <stddef.h>
#include <stdio.h> // for puts
#include <string.h> // for memcpy, memset
#include <sys/socket.h> // for socklen_t, recv
#include <sys/types.h> // for ssize_t

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
  _delete = true;
}

// Send & Receive
//processData is a smaller type of serve() until conditions are fully implemented
void Connection::processData(void) {
	if(!_req.ClientHungUp && !_req.isFullyParsed())
		_req.process(_sock);
	
	if (_req.ClientHungUp){
		scheduleForDemolition();
		return ;
	};
		 
	//parsing from buffer into Request
	//when fully parsed init response 
	if(_req.isFullyParsed())
	{
		_res.init(_req);
		int dummy = -1;
		while (!_res.process(_sock, dummy, BYTES_PER_CHUNK))
  			 ;	}
}

bool Connection::serve(const size_t Bytes) {
  //handle Request until fully parsed
	if (!_req.isFullyParsed()) {
		if (_conditionsFulfilled & _req.getConditions())
			_req.process(_sock);
		if (_req.isFullyParsed())
			_res.init(_req);
    return false;
  }
  //switch to Response	
  if (_conditionsFulfilled & _res.getConditions())
    return _res.process(_sock, _sockForward, Bytes);
  return false;
}

