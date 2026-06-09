
#include "Config.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Server.hpp"
#include "Website.hpp"
#include <cstdlib>
#include <exception>
#include <list>

#define DEFAULT_CONFIG "test_configs/config.txt"

int main(int argc, char **argv) {

  const char *configPath = DEFAULT_CONFIG;
  if (argc >= 2)
    configPath = argv[1];

  try {
    const Config conf(configPath);
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

