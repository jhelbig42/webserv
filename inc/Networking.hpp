#pragma once

#include "Logging.hpp"
#include "Connection.hpp"
#include "Config.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring> // for memset
#include <cstdlib> // for exit
#include <stdexcept> // for runtime_error
#include <unistd.h> // for close

namespace networking {

	void	start(void);	
	struct addrinfo create_hints(void);
	struct  addrinfo *get_server_info(void);
	std::string    get_addrinfo_str(struct addrinfo *info, std::string msg);
	void    print_addrinfo_str(struct addrinfo *info);
	void    fill_addrinfo(char *node, struct addrinfo *hints, struct addrinfo *info);

}

