#include "Autoindex.hpp"
#include "Config.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include <iostream>
//#define _GNU_SOURCE 
#include "Config.hpp"
#include "Server.hpp"
#include <iostream>
#include <unistd.h>

//#define OFFLINE
//#define PARSING
//#define AUTOINDEX

#define CHUNK_SIZE 1024
#define DEFAULT_CONFIG "test_configs/config.txt"

#define METHOD "GET"
#define PATH "/home/julia/projects/webserv/hello.txt"
#define VERSION "HTTP/1.0"

int main(int argc, char **argv) {

  const char *configPath = DEFAULT_CONFIG;
  if (argc >= 2)
    configPath = argv[1];

  try {
    const Config conf(configPath);
	const std::list<Website> &websites = conf.getWebsites();
	if (websites.empty()){
    throw std::runtime_error("config file contains 0 websites");
	}
	Server server(websites);
	server.pollLoop();
  } catch (const std::exception &e) {
  	logging::log(logging::Error, e.what());
	exit (1);
  	}
}

