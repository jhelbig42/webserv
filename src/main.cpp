#include "Logging.hpp"
#include "Networking.hpp"
#include <iostream>
#include "Response.hpp"
#include "Request.hpp"
#include <unistd.h>

#define BYTES_PER_CHUNK 256

int main(void) {

  try { 
  	networking::start();
  }
  catch (const std::runtime_error & e){
  	logging::log(logging::Error, e.what());
  }
  
  try {
  Request req("GET /home/julia/projects/webserv/Makefile HTTP/1.0");
  //Request req(Get, "/home/julia/projects/webserv/Makefile", 0, 9, true);
  Response res(req);
 
    while (!res.process(STDOUT_FILENO, BYTES_PER_CHUNK))
    {
      logging::log(logging::Debug, req.getResource());
      if (req.isValid() != true)
        logging::log(logging::Error, "Request invalid");
    }
  } 
  catch (std::exception &e)
  {
    logging::log(logging::Error, e.what());
    return 1;
  }
  return 0;
}
