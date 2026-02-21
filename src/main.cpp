#include "Logging.hpp"
#include "Networking.hpp"
#include <iostream>
#include "Request.hpp"
#include "Response.hpp"
#include <exception>
#include <unistd.h>

#define OFFLINE

#define CHUNK_SIZE 1024

#define METHOD "GET"
#define PATH "/data/data/com.termux/files/home/code/webserv/.gitignore"
#define VERSION "HTTP/1.0"

#ifdef OFFLINE

int main(void) {
	Request req(METHOD " " PATH " " VERSION);
	Response res(req);
	while (!res.process(STDOUT_FILENO, -1, CHUNK_SIZE))
		;
}

#else	

int main(void) {

  try { 
  	networking::start();
  }
  catch (const std::runtime_error & e){
  	logging::log(logging::Error, e.what());
  }
}

#endif // OFLINE
