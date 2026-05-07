/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:19:03 by hallison          #+#    #+#             */
/*   Updated: 2026/03/24 11:14:01 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conditions.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "NetworkingDefines.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include <cerrno> // for errno
#include <cstring> // for strerror
#include <stddef.h>
#include <string.h>     // for memcpy, memset
#include <sys/socket.h> // for socklen_t, recv

// Construct & Destruct

Connection::Connection(const int Sock, const sockaddr_storage &Addr,
                       const socklen_t Addr_size, const Website &website)
    : _conditionsWanted(SockRead), _sock(Sock), _website(website), _delete(false), _addrSize(sizeof _addr)  {

  memset(&_info, 0, sizeof _info); // unneccessary? delete?
  memcpy(&_addr, &Addr, Addr_size);
  memset(&_readBuf, 0, MAX_REQUEST);
  _timeLastActive = time(NULL);
  logging::log(logging::Debug, "Connection created");
}

// Getters

Connection::~Connection(void) {
}

int Connection::getSock(void) const {
  return (_sock);
}

int Connection::getSockForward(void) const{
	return (_sockForward);
}

bool Connection::getDeleteStatus(void) const {
  return (_delete);
}

time_t Connection::getTimeLastActive(void) const {
  return (_timeLastActive);
}

/*
Conditions Connection::getConditions(void) const {
  if (_req.getState() == COMPLETE)
    return _react.getConditions();
  return _req.getConditions();
}
*/

int Connection::getConditionsWanted(void) const {
  return _conditionsWanted;
}

// Setters
void Connection::scheduleForDemolition(void) {
  _delete = true;
}

void Connection::setTimeLastActive(time_t Time) {
  _timeLastActive = Time;
}

void Connection::updateConditionsWanted(Reaction::ProcessType ProcessType){
	switch (ProcessType){
		case Reaction::SendFile:
			_conditionsWanted = SockWrite;
			break;
		case Reaction::ReceiveFile:
			_conditionsWanted = SockRead;
			break;
		case Reaction::CgiPost:
			_conditionsWanted = SockWrite | SockRead | FSockWrite | FSockRead;
			break;
		case Reaction::CgiNotPost:
			_conditionsWanted = SockWrite | FSockRead;
			break;
		case Reaction::NotInitialized:
		default:
			_conditionsWanted = SockRead;

	}
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
		_react.setPathInfo(_website.getPathInfo(_req.getResource())); // setting config details into Reaction
		_react.init(_req, _sock);
	}
	// we have a initialized Reaction - act on it.
	//we do not need the CGI sockets handed over here, as they are set in Reaction itself
	if(_react.process(_sock, BYTES_PER_CHUNK, _conditionsFulfilled)) // returns only true if the creation and sending of the process is done
		scheduleForDemolition();
	//update ConditionsWanted here - from Reaction
	//before process is called in the next round
	updateConditionsWanted(_react.getProcessType());
}

// Conditions

void Connection::addToConditions(Conditions Condition) {
  _conditionsFulfilled = _conditionsFulfilled | Condition;
}

void Connection::resetConditions(void) {
  _conditionsFulfilled = 0;
}
