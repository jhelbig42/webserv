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
#include "Reaction.hpp"
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

// Setters

void Connection::scheduleForDemolition(void) {
  _delete = true;
}

void Connection::serve(void) {
	//if request is not yet complete, read and parse until it is
	if( (_conditionsFulfilled & SockRead) 
		&& _req.getState() != COMPLETE && _req.getState() != INVALID)
		_req.process(_sock);
	if (_req.getState() == CLIENTHUNGUP){
		scheduleForDemolition();
		return ;
	};
	// if Request is complete, reaction can get initialized - NO Socket Access Required
	// need a not initialized state for Reaction here
	if((_req.getState() == COMPLETE || _req.getState() == INVALID) 
		&& _react.getProcessType() == Reaction::NotInitialized)
	{
		_react.init(_req);
		_req.reset();
	}
	// we have a initialized Reaction - act on it.
	int dummy = -1;
	if(_react.process(_sock, dummy, BYTES_PER_CHUNK, _conditionsFulfilled)) // returns only true if the creation and sending of the process is done
		scheduleForDemolition();
}

// Conditions

void Connection::addToConditions(Conditions Condition) {
  _conditionsFulfilled = _conditionsFulfilled | Condition;
}

void Connection::resetConditions(void) {
  _conditionsFulfilled = 0;
}
