/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionOld.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 16:48:05 by hallison          #+#    #+#             */
/*   Updated: 2026/02/23 15:27:56 by hallison         ###   ########.fr       */
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
  Connection(const int sock, const sockaddr_storage &addr,
             const socklen_t addr_size);
  ~Connection();

  bool serve(const size_t Bytes);
  Conditions getConditions() const;

  // Marked for destruction
  bool _delete;
  // making public for now, could also be private with getter

  // getters
  int get_sock(void) const;

  // setters
  void schedule_for_demolition(void);

  // read from socket
  void read_data(void);

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
