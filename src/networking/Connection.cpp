/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 15:29:31 by hallison          #+#    #+#             */
/*   Updated: 2026/02/19 16:35:21 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logging.hpp"
#include "Connection.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <stdio.h> // for puts

#define BYTES_PER_CHUNK 256


// Construct & Destruct

Connection::Connection(const int sock, const sockaddr_storage &addr, const socklen_t addr_size)
    : _sock(sock), _addr_size(sizeof _addr) {

  memset(&_info, 0, sizeof _info); // unneccessary? delete?
  memcpy(&_addr, &addr, addr_size);
  memset(&_read_buf, 0, MAX_REQUEST);
  logging::log(logging::Debug, "Connection created");
}

Connection::~Connection(void) {}

// getters

int Connection::get_sock(void) const { return (_sock); }


void Connection::read_data(void){

	logging::log(logging::Debug, "read_data()\n");
	ssize_t bytes_read = recv(_sock, &_read_buf, MAX_REQUEST, 0);
	
	if (bytes_read == MAX_REQUEST){
		logging::log(logging::Info,
			"read_data(): bytes_read == MAX REQUEST");
			// May happen frequently, will be handled in chunks
			// Logging for debug purposes as we build.
	}
	if (bytes_read == 0){
		logging::log(logging::Warning,
			"read_data(): bytes_read == 0");
		return;
			// This can indicate client hangup.
			// We will need to remove client from fds & connections
			// likely by setting a flag in Connection that networking
			// loop will handle
	}
	if (bytes_read < 0) {
		std::ostringstream msg;
		msg << "recv: " << std::strerror(errno);
		logging::log(logging::Warning, msg.str());
		return;
	}
	_read_buf[bytes_read - 1] = '\0';
//	puts(_read_buf);
	//_req.init("GET /home/hallison/webserv/.gitignore HTTP/1.0");
	_req.init(_read_buf);
	_res.init(_req);
	  while (!_res.process(_sock, BYTES_PER_CHUNK))
		;
	logging::log(logging::Debug, "read_buf = ");
	logging::log(logging::Debug, _read_buf);
}
