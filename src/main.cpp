#include "Logging.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <exception>
#include <unistd.h>

#define BYTES_PER_CHUNK 256

int main(void) {
  Request const req(Get, "/home/alneuman/projects/webserv/Makefile", 1, 0, true);
  Response res(req);
  try {
    while (!res.process(STDOUT_FILENO, BYTES_PER_CHUNK))
      logging::log("hi", logging::Error);
  } catch (std::exception &e) {
    logging::log(e.what(), logging::Error);
    return 1;
  }
  return 0;
}
