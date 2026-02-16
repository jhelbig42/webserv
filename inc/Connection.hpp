/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 16:48:05 by hallison          #+#    #+#             */
/*   Updated: 2026/02/13 17:16:04 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Logging.hpp"
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
		int _index;
		bool	_active; // in use or not?
		int		_sock; // client socket fd

		struct	addrinfo _info;
		struct sockaddr_storage _addr; // client's IP 
		socklen_t	_addr_size;

	public:
		Connection *next;
		Connection();
		Connection(int server_sock);
		Connection(const int sock, const sockaddr_storage &addr, const socklen_t addr_size);
		~Connection();
		void print_addrinfo(void) const;
		
		// getters
		bool is_active(void) const;
		int get_sock(void) const;
		socklen_t get_addr_size(void) const;
		
		// setters
		void clear_connection(void); // reset all fields to empty
		int fill_request_buffer(char *src, int bytes);
		int accept_new(int sock);

		// input buffer
		std::vector<char> read_buf;
};
