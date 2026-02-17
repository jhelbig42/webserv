#include "Logging.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <unistd.h>

#define BYTES_PER_CHUNK 256

int main(void) {
  try {
  Request req("GET /home/julia/projects/webserv/Makefile HTTP/1.0");
  //Request req(Get, "/home/julia/projects/webserv/Makefile", 0, 9, true);
  Response res(req);
 
    while (!res.process(STDOUT_FILENO, BYTES_PER_CHUNK))
    {
      logging::log(req.getResource(), logging::Debug);
      if (req.isValid() != true)
        logging::log("Request invalid", logging::Error);
    }
  } 
  catch (std::exception &e)
  {
    logging::log(e.what(), logging::Error);
    return 1;
  }
  return 0;
}
