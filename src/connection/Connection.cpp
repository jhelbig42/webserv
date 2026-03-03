/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:19:03 by hallison          #+#    #+#             */
/*   Updated: 2026/02/27 12:32:38 by hallison         ###   ########.fr       */
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
    : _sock(Sock), _delete(false), _addrSize(sizeof _addr) {

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

bool Connection::getDeleteStatus(void) const {
	return (_delete);
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
//processData is a smaller type of serve() until conditions are fully implemented
void Connection::processData(void) {
	if(_req.getState() != COMPLETE && _req.getState() != INVALID)
		_req.process(_sock);
	
	if (_req.getState() == CLIENTHUNGUP){
		scheduleForDemolition();
		return ;
	};
		 
	//parsing from buffer into Request
	//when fully parsed init response 
	if(_req.getState() == COMPLETE || _req.getState() == INVALID)
	{
		_res.init(_req);
		_req.reset();
		int dummy = -1;
		while (!_res.process(_sock, dummy, BYTES_PER_CHUNK))
  			 ;	}
		
		if (_req.getState() == CLIENTHUNGUP){
			scheduleForDemolition();
			return ;
	};
}

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

// Conditions

void Connection::addToConditions(Conditions Condition) {
  _conditionsFulfilled = _conditionsFulfilled | Condition;
}

void Connection::resetConditions(void) {
  _conditionsFulfilled = 0;
}
