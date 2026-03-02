#include "Logging.hpp"
#include "Networking.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <stdexcept>


//#define OFFLINE

#define CHUNK_SIZE 1024

#define METHOD "GET"
#define PATH "/home/julia/projects/webserv/hello.txt"
#define VERSION "HTTP/1.0"

// GET /home/julia/projects/webserv/hello.txt HTTP/1.0
// GET /home/jhelbig/Dektop/webserv/hello.txt HTTP/1.0
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

#else

int main(void) {

  try {
    networking::start();
  } catch (const std::runtime_error &e) {
    logging::log(logging::Error, e.what());
  }
}

#endif // OFLINE
