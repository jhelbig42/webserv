/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Networking.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hallison <hallison@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:52:19 by hallison          #+#    #+#             */
/*   Updated: 2026/02/10 18:38:14 by hallison         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

void	fill_hints(struct addrinfo *hints);
int		get_info(char *node, struct addrinfo *hints, struct addrinfo *info);
void	print_addrinfo(struct addrinfo *info);

void	init_networking(void){

	struct addrinfo hints, *info, *p;
	fill_hints(&hint);
}

void	fill_hints(strict addrinfo *hints){
	memset(hints, 0, sizeof *hints); // init struct to empty;
	hints->ai_family = AF_UNSPEC; // allows either IPv4 or IPv6
	hints->ai_socktype = SOCK_STREAM; // for TCP stream sockets
	hints->ai_flags = AI_PASSIVE; // autofill IP
}

void	print_addrinfo(struct addrinfo *info){

        std::cout << "ai_flags = " << info->ai_flags << "\n";
        std::cout << "ai_family = " << info->ai_family << "\n";
        std::cout << "ai_socktype = " << info->ai_socktype << "\n";
        std::cout << "ai_protocol = " <<  info->ai_protocol << "\n";
        std::cout << "ai_addrlen = " << info->ai_addrlen << "\n";

}
