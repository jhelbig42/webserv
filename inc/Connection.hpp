/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 16:48:05 by hallison          #+#    #+#             */
/*   Updated: 2026/02/10 17:15:15 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class	Connection {

	private:
		int	socket_descriptor; // socket fd
		struct	addrinfo info;
		// TODO OFC
		// TODO OFC
	public:
		Connection();
		~Connection();
		int get_sock(void) const;
		void print_addrinfo(void) const;

		// There will later be many getters for addrinfo
};
