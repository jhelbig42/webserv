#include "Config.hpp"
#include "Networking.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "Scanner.hpp"
#include <iostream>
#include <iostream>
#include <unistd.h>

//#define OFFLINE
#define PARSING

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

int main(void) {

  try {
    networking::start();
  } catch (const std::runtime_error &e) {
    logging::log(logging::Error, e.what());
  }
}

#endif // OFLINE
