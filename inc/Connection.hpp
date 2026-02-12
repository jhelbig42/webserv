/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 16:48:05 by hallison          #+#    #+#             */
/*   Updated: 2026/02/12 17:28:27 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class	Connection {
	private:
		bool	active; // in use or not?
		int		socket_descriptor; // socket fd
		struct	addrinfo info;
		socklen_t	addr_size;
	public:
		Connection();
		~Connection();
		int get_sock(void) const;
		void print_addrinfo(void) const;
		
		// There will later be many getters for addrinfo
};
