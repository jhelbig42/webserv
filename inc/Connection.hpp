/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:00:20 by hallison          #+#    #+#             */
/*   Updated: 2026/02/23 17:24:16 by hallison         ###   ########.fr       */
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
  
  Connection(const int sock, const sockaddr_storage &addr,
             const socklen_t addr_size);
  ~Connection();

  // getters
  int get_sock(void) const;

  // setters
  void schedule_for_demolition(void);

  // send & receive
  void read_data(void);
  bool serve(const size_t Bytes);
  Conditions getConditions() const;

private:
  Connection(); // should not be possible


  // serve reads from
  Conditions _conditionsFulfilled;

  int _sock; // client socket fd
  int _sockForward;

  // networking
  struct addrinfo _info;
  struct sockaddr_storage _addr; // client's IP
  socklen_t _addr_size;

  Request _req;
  Response _res;
  
  char _read_buf[MAX_REQUEST];
};
