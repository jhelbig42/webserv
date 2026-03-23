/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:00:20 by hallison          #+#    #+#             */
/*   Updated: 2026/03/23 17:27:30 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Conditions.hpp"
#include "NetworkingDefines.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "Website.hpp"
#include <netdb.h>
#include <sys/socket.h>

class Connection {
public:
   
  Connection(const int Sock, const sockaddr_storage &Addr,
             const socklen_t addrSize, const Website &website);
  ~Connection();

  // getters
  int getSock(void) const;
  bool getDeleteStatus(void) const;

  // setters
  void scheduleForDemolition(void);
  void addToConditions(Conditions Condition);
  void resetConditions(void);

  // send & receive
  //void readData(void);
  bool serve(const size_t Bytes);
  void serve(void);
  Request _req;
  Reaction _react;

private:
  Connection(); // should not be possible

  // serve reads from
  int _conditionsFulfilled;

  int _sock; // client socket fd
  int _sockForward;
  const Website &_website;

  // networking
  bool _delete; // set by scheduleForDemolition
  struct addrinfo _info;
  struct sockaddr_storage _addr; // client's IP
  socklen_t _addrSize;
  
  Buffer	_buf;
  char		_readBuf[MAX_REQUEST];
};
