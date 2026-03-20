/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:19:03 by hallison          #+#    #+#             */
/*   Updated: 2026/03/20 16:56:45 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conditions.hpp"
#include "Connection.hpp"
#include "Logging.hpp"
#include "NetworkingDefines.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
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
                       const socklen_t Addr_size, const Website &website)
    : _sock(Sock), _delete(false), _addrSize(sizeof _addr), _website(website) {

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
    return _react.getConditions();
  return _req.getConditions();
}

// Setters

void Connection::scheduleForDemolition(void) {
  _delete = true;
}

// Send & Receive
// processData is a smaller type of serve() until conditions are fully
// implemented
void Connection::processData(void) {
  if ((_conditionsFulfilled & _req.getConditions()) &&
      _req.getState() != COMPLETE && _req.getState() != INVALID)
    _req.process(_sock);
  if (_req.getState() == CLIENTHUNGUP) {
    scheduleForDemolition();
    return;
  };
  // parsing from buffer into Request
  // when fully parsed init Reaction
  // Reaction will set its conditions
  if (_conditionsFulfilled == SockWrite &&
      (_req.getState() == COMPLETE || _req.getState() == INVALID))

    // if Request is complete, reaction can get initialized - NO Socket Access
    // Required need a not initialized state for Reaction here

    if ((_req.getState() == COMPLETE || _req.getState() == INVALID) &&
        _react.getProcessType() == Reaction::NotInitialized) {
      _react.init(_req);
      _req.reset();
    }
  // if reaction does not need more aka is not POST or POST is done, then
  // process
  // this needs write access to the socket
  if (_conditionsFulfilled & _react.getConditions()) {
    int dummy = -1;

    if (_react.process(_sock, dummy, BYTES_PER_CHUNK) == true) {

      logging::log(logging::Debug, "finished writing to client");
      scheduleForDemolition();
      // close after finishing writing greatly simplifies
      // polling & is consistent with HTTP 1.0
    }

    // while (!_react.process(_sock, dummy, BYTES_PER_CHUNK)){
    //	 ;	}
  }
}

bool Connection::serve(const size_t Bytes) {
  // handle Request until fully parsed
  if (_req.getState() != COMPLETE) {
    if (_conditionsFulfilled & _req.getConditions())
      _req.process(_sock);
    if (_req.getState() == COMPLETE)
      _react.init(_req);
    return false;
  }
  // switch to Reaction
  if (_conditionsFulfilled & _react.getConditions())
    return _react.process(_sock, _sockForward, Bytes);
  return false;
}

// Conditions

void Connection::addToConditions(Conditions Condition) {
  _conditionsFulfilled = _conditionsFulfilled | Condition;
}

void Connection::resetConditions(void) {
  _conditionsFulfilled = 0;
}
