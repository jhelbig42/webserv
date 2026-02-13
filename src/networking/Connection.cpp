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

Connection::Connection(void): _index(-42), _active(false), _sock(-1), _addr_size(sizeof _addr), next (NULL){
	memset(&_info, 0, sizeof _info);
	memset(&_request_buffer, '\0', sizeof _request_buffer);
}

Connection::Connection(int server_sock): _index(-42), _active(false), _sock(-1), _addr_size(sizeof _addr){
	
	memset(&_info, 0, sizeof _info); // unneccessary? delete?
	memset(&_request_buffer, '\0', sizeof _request_buffer);

	_sock = accept(server_sock, (struct sockaddr *)&_addr, &_addr_size);
	if (_sock == -1){
		std::ostringstream msg;
		msg << "accept: " << std::strerror(errno) <<
			" (may continue trying to accept connections)";
			throw std::runtime_error(msg.str()); // is it safe to throw exception in constructor?
			// or do we need a function that performs accept & checks validity of results?
	}
	else {
		_active = true;
		std::cout << "\nConnection accepted on socket " << _sock << "\n\n";
//		logging::log3(logging:Debug, "Connection accepted on socket ", _sock, "\n");
	}
}

Connection::Connection(int sock, sockaddr_storage &addr, socklen_t addr_size): _index(-42), _active(true), _sock(sock), _addr_size(sizeof _addr){
	
	memset(&_info, 0, sizeof _info); // unneccessary? delete?
	memset(&_request_buffer, '\0', sizeof _request_buffer);
	memcpy(&_addr, &addr, addr_size);

	logging::log(logging::Debug, "Connection created");
}
	
Connection *Connection::create_connection(int server_sock){
	
	struct sockaddr_storage addr;
	int		client_sock;
	socklen_t addr_size = sizeof addr;

	client_sock = accept(server_sock, (struct sockaddr *)&addr, &_addr_size);
	if (client_sock == -1){
		std::ostringstream msg;
		msg << "accept: " << std::strerror(errno) <<
			" (can continue trying to accept connections)";
		logging::log(logging::Error, msg.str()); // may downgrade log level at some point
		return (NULL);
	}
	Connection *new_connection = new Connection(client_sock, addr, addr_size);
	std::cout << "\nConnection accepted on socket " << _sock << "\n\n";
//	logging::log3(logging:Debug, "Connection accepted on socket ", _sock, "\n");
	return (new_connection);
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
