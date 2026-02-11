#pragma once

#include "Connection.hpp"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring> // for memset
#include <cstdlib> // for exit

namespace networking {

	void	start(void);	
	struct addrinfo create_hints(void);
	struct  addrinfo *get_server_info(void);
	void    print_addrinfo(struct addrinfo *info);
	void    fill_addrinfo(char *node, struct addrinfo *hints, struct addrinfo *info);

}

