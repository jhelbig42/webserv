/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 15:29:31 by hallison          #+#    #+#             */
/*   Updated: 2026/02/13 15:35:09 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

// Construct & Destruct

Connection::Connection(void): _index(-42), _active(false), _sock(-1), _addr_size(sizeof _addr){
	memset(&_info, 0, sizeof _info);
	memset(&_request_buffer, '\0', sizeof _request_buffer);
}

Connection::~Connection(void){}

// getters

int	Connection::get_sock(void) const{
	return (_sock);
}

// setters

void Connection::clear_connection(){
	_active = false;
	_sock = -1;
	_addr_size = sizeof _addr;
	memset(&_info, 0, sizeof _info);
	memset(&_request_buffer, '\0', sizeof _request_buffer);
}

// accept

int Connection::accept_new(int server_sock){
	_sock = accept(server_sock, (struct sockaddr *)&_addr, &_addr_size);
	if (_sock == -1){
		std::ostringstream msg;
		msg << "accept: " << std::strerror(errno) <<
			" (may continue trying to accept connections)";
			logging::log(logging::Error, msg.str());
	}
	else {
		_active = true;
	}
	return (_sock);
}
