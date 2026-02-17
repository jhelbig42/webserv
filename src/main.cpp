#include "Logging.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <exception>
#include <unistd.h>

#define METHOD "GET"
#define PATH "/home/alneuman/projects/webserv/.gitignore"
#define VERSION "HTTP/1.0"

#define BYTES_PER_CHUNK 256

int main(void) {
  try {
  const Request req(METHOD " " PATH " " VERSION);
  Response res(req);
  while (!res.process(STDOUT_FILENO, BYTES_PER_CHUNK))
    ;
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
    return 1;
  }
  return 0;
}
