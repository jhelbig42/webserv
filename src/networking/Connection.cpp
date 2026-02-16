/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 15:29:31 by hallison          #+#    #+#             */
/*   Updated: 2026/02/13 17:19:02 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

// Construct & Destruct

/*
Connection::Connection(void): _index(-42), _active(false), _sock(-1),
_addr_size(sizeof _addr), next (NULL){ memset(&_info, 0, sizeof _info);
        memset(&_request_buffer, '\0', sizeof _request_buffer);
}

Connection::Connection(int server_sock): _index(-42), _active(false), _sock(-1),
_addr_size(sizeof _addr){

        memset(&_info, 0, sizeof _info); // unneccessary? delete?
        memset(&_request_buffer, '\0', sizeof _request_buffer);

        _sock = accept(server_sock, (struct sockaddr *)&_addr, &_addr_size);
        if (_sock == -1){
                std::ostringstream msg;
                msg << "accept: " << std::strerror(errno) <<
                        " (may continue trying to accept connections)";
                        throw std::runtime_error(msg.str()); // is it safe to
throw exception in constructor?
                        // or do we need a function that performs accept &
checks validity of results?
        }
        else {
                _active = true;
                std::cout << "\nConnection accepted on socket " << _sock <<
"\n\n";
//		logging::log3(logging:Debug, "Connection accepted on socket ",
_sock, "\n");
        }
}
*/

Connection::Connection(const int sock, const sockaddr_storage &addr, const socklen_t addr_size)
    : _index(-42), _active(true), _sock(sock), _addr_size(sizeof _addr) {

  memset(&_info, 0, sizeof _info); // unneccessary? delete?
  memcpy(&_addr, &addr, addr_size);

  logging::log(logging::Debug, "Connection created");
}

Connection::~Connection(void) {}

// getters

int Connection::get_sock(void) const { return (_sock); }

// setters

void Connection::clear_connection() {
  _active = false;
  _sock = -1;
  _addr_size = sizeof _addr;
  memset(&_info, 0, sizeof _info);
}

// accept

int Connection::accept_new(int server_sock) {
  _sock = accept(server_sock, (struct sockaddr *)&_addr, &_addr_size);
  if (_sock == -1) {
    std::ostringstream msg;
    msg << "accept: " << std::strerror(errno)
        << " (may continue trying to accept connections)";
    logging::log(logging::Error, msg.str());
  } else {
    _active = true;
  }
  return (_sock);
}
