#include "Logging.hpp"

#define BYTES_PER_CHUNK 256

int main(void) {
  Request req(Get, "/home/alneuman/projects/webserv/Makefile", 1, 0, true);
  Response res(req);
  try {
    while (!res.process(STDOUT_FILENO, BYTES_PER_CHUNK))
      ;
  } catch (std::exception &e) {
    logging::log(e.what(), logging::Error);
    return 1;
  }
  return 0;
}
