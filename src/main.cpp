#include "Scanner.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Reaction.hpp"
#include "Networking.hpp"
#include "Config.hpp"
#include <unistd.h>
#include <iostream>

//#define OFFLINE
//#define PARSING

#define CHUNK_SIZE 1024

#define METHOD "GET"
#define PATH "/home/julia/projects/webserv/hello.txt"
#define VERSION "HTTP/1.0"

// GET /home/julia/projects/webserv/hello.txt HTTP/1.0
// GET /home/jhelbig/Desktop/webserv/hello.txt HTTP/1.0
//home/jhelbig/Desktop/webserv

// Content_Length: 100
#ifdef OFFLINE

int main(void) {
  const Request req(METHOD " " PATH " " VERSION);
  Reaction res(req);
  int dummy = -1;
  while (!res.process(STDOUT_FILENO, dummy, CHUNK_SIZE))
    ;
}

#elif defined PARSING

int main(int argc, char **argv) {
  if (argc != 2)
    return 1;
  try {
    const Config conf(argv[1]);
    std::cout << conf;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  // const Scanner scan(argv[1]);
  // std::cout << scan;
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

	// first instantiate websites, check that they exist?
    Server server(conf);
	server.pollLoop();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
	exit (1);
  }
}

#endif // OFLINE
