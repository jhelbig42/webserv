#include "Scanner.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Networking.hpp"
#include <unistd.h>
#include <iostream>

//#define OFFLINE
#define PARSING

#define CHUNK_SIZE 1024

#define METHOD "HEAD"
#define PATH "/home/alneuman/projects/webserv/hello.txt"
#define VERSION "HTTP/1.0"

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
  const Scanner scan(argv[1]);
  std::cout << scan;
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
