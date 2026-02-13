/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 16:48:05 by hallison          #+#    #+#             */
/*   Updated: 2026/02/13 13:29:49 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h> // for memset

class	Connection {
	private:
		int _index;
		bool	_active; // in use or not?
		int		_sock; // client socket fd
		struct	addrinfo _info;
		socklen_t	_addr_size;
		char _request_buffer[1024];
	public:
		Connection();
		~Connection();
		void clear_connection(void);
		int get_sock(void) const;
		void print_addrinfo(void) const;
		
		// There will later be many getters for addrinfo
};
