#include "Config.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "Scanner.hpp"
#include <iostream>
//#define _GNU_SOURCE 
#include "Config.hpp"
#include "Server.hpp"
#include <iostream>
#include <unistd.h>

//#define OFFLINE
//#define PARSING

#define CHUNK_SIZE 1024

#define METHOD "GET"
#define PATH "/home/julia/projects/webserv/hello.txt"
#define VERSION "HTTP/1.0"

// GET /home/julia/projects/webserv/hello.txt HTTP/1.0
// GET /home/jhelbig/Desktop/webserv/hello.txt HTTP/1.0
// GET /home/jhelbig/Desktop/webserv/scripts/test_query.py?user=max&type=dog HTTP/1.0
//home/jhelbig/Desktop/webserv

// Content_Length: 100
#ifdef OFFLINE

int main(void) {
  const Request req(METHOD " " PATH " " VERSION);
  Reaction res(req);
  while (!res.process(STDOUT_FILENO, CHUNK_SIZE, Unconditional))
    ;
}

#elif defined PARSING

int main(int argc, char **argv) {
  if (argc != 3 || argv[2][0] != '/')
    return 1;
  // const Scanner scan(argv[1]);
  // std::cout << scan;
  try {
    const Config conf(argv[1]);
    std::cout << conf;

    if (conf.getWebsites().begin() != conf.getWebsites().end()) {
      PathInfo info = conf.getWebsites().begin()->getPathInfo(argv[2]);
      std::cout << info;
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return 0;
}

#else

int main(int argc, char **argv) {

  // argument check
  if (argc != 2){
  	std::cerr << "Usage: <config file>\n";
	exit (1);
  }
  
  try {
    const Config conf(argv[1]);
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

#endif // OFLINE
