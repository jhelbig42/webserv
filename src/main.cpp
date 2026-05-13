#include "Autoindex.hpp"
#include "Config.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include <iostream>
#include "Config.hpp"
#include "Server.hpp"
#include <iostream>
#include <unistd.h>

#define DEFAULT_CONFIG "test_configs/config.txt"

//#define PARSING

#ifdef OFFLINE

int main(void) {
  const Request req(METHOD " " PATH " " VERSION);
  Reaction res(req);
  while (!res.process(STDOUT_FILENO, CHUNK_SIZE, Unconditional))
    ;
}

#elif defined PARSING

int main(int argc, char **argv) {
  if (argc != 3 || argv[2][0] != '/')
    return 1;
  try {
    const Config conf(argv[1]);
    std::cout << conf;

    if (conf.getWebsites().begin() != conf.getWebsites().end()) {
      PathInfo info = conf.getWebsites().begin()->getPathInfo(argv[2]);
      std::cout << info;
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return 0;
}

#elif defined AUTOINDEX
int main(int argc, char **argv){
	std::cout << "Autoindex test" << std::endl;
	if (argc != 2)
		return 1;
	Autoindex a;
	std::cout << a.autoindexStream(argv[1], argv[1]);
}

#else

int main(int argc, char **argv) {

  std::string input;
  // argument check
  if (argc == 1)
      input = DEFAULT_CONFIG;
  else if (argc == 2)
      input = argv[1];
  else{
  	std::cerr << "Usage: <config file>\n";
	  exit (1);
  }
  
  try {
    const Config conf(input.c_str());
	  const std::list<Website> &websites = conf.getWebsites();
	  if (websites.empty()){
      throw std::runtime_error("config file contains 0 websites");
	}
	Server server(websites);
	server.pollLoop();
  } catch (const std::exception &e) {
  	logging::log(logging::Error, e.what());
	exit (1);
  }
}

#endif
