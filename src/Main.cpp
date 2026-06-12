#include "Config.hpp"
#include "Logging.hpp"
#include "Server.hpp"
#include "Signals.hpp"
#include "Website.hpp"
#include <cstdlib>
#include <exception>
#include <list>

#define DEFAULT_CONFIG "configs/config.conf"

int main(int argc, char **argv) {

  if (registerSigint() < 0)
    return 1;

  const char *configPath = DEFAULT_CONFIG;
  if (argc >= 2)
    configPath = argv[1];

  try {
    const Config conf(configPath);
    const std::list<Website> &websites = conf.getWebsites();
    if (websites.empty()) {
      throw std::runtime_error("config file contains 0 websites");
    }
    Server server(websites);
    if (server.pollLoop() < 0)
      return 1;
  } catch (const std::exception &e) {
    logging::log(logging::Error, e.what());
    return 1;
  }
  return 0;
}
