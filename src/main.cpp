#include "Logging.hpp"
#include "Networking.hpp"
#include <iostream>
#include "Request.hpp"
#include "Response.hpp"
#include <exception>
#include <unistd.h>

#define METHOD "GET"
#define PATH "/home/alneuman/projects/webserv/.gitignore"
#define VERSION "HTTP/1.0"


int main(void) {

  try { 
  	networking::start();
  }
  catch (const std::runtime_error & e){
  	logging::log(logging::Error, e.what());
  }
}
