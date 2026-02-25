/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:00:20 by hallison          #+#    #+#             */
/*   Updated: 2026/02/25 15:48:57 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Conditions.hpp"
#include "NetworkingDefines.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <netdb.h>
#include <sys/socket.h>

class Connection {
public:
  
  bool _delete; // placement in public is temporary
  
  Connection(const int Sock, const sockaddr_storage &Addr,
             const socklen_t addrSize);
  ~Connection();

  // getters
  int getSock(void) const;

  // setters
  void scheduleForDemolition(void);
  void addToConditions(Conditions Condition);
  void resetConditions(void);

  // send & receive
  void readData(void);
  bool serve(const size_t Bytes);
  Conditions getConditions() const;

private:
  Connection(); // should not be possible


  // serve reads from
  int _conditionsFulfilled;

  int _sock; // client socket fd
  int _sockForward;

  // networking
  struct addrinfo _info;
  struct sockaddr_storage _addr; // client's IP
  socklen_t _addrSize;

  Request _req;
  Response _res;
  
  char _readBuf[MAX_REQUEST];
};
