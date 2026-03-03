#include "Scanner.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Networking.hpp"
#include <unistd.h>
#include <iostream>

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
  Response res(req);
  int dummy = -1;
  while (!res.process(STDOUT_FILENO, dummy, CHUNK_SIZE))
    ;
}

#elif defined PARSING

int main(int argc, char **argv) {
  if (argc != 2)
    return 1;
  const Scanner scan(argv[1]);
  std::cout << scan;
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
