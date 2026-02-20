/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 16:48:05 by hallison          #+#    #+#             */
/*   Updated: 2026/02/20 13:57:00 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Logging.hpp"
#include "NetworkingDefines.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream> // for temp debug print in constructor
#include <string.h> // for memset
#include <cerrno> // for errno
#include <cstring> // for std::strerror
#include <string.h> // for memcpy
#include <vector>

class	Connection {
	private:
		int		_sock; // client socket fd
		char	_read_buf[MAX_REQUEST]; // or vector?
		bool	_delete;

		// Relevant to accept(), maybe more
		struct	addrinfo _info;
		struct sockaddr_storage _addr; // client's IP 
		socklen_t	_addr_size;
		
		Connection(); // should not be possible

	public:
		Connection(const int sock, const sockaddr_storage &addr, const socklen_t addr_size);
		~Connection();

		Request _req;
		Response _res;
		
		// getters
		int get_sock(void) const;

		// setters
		void schedule_for_demoltion(void);
		
		// read from socket
		void read_data(void);
};
