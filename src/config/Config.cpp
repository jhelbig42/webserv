#include "Config.hpp"

#include "ConfigDefaults.hpp"
#include "Logging.hpp"
#include <string>

/// \var static logging::LogLevel globalLogLevel
/// \brief defines the lowest log severity that is still printed
static logging::LogLevel globalLogLevel = LOG_LEVEL;

/// \var static bool globalLogColored
/// \brief defines if log messages are colored
static bool globalLogColored = LOG_COLORED;

logging::LogLevel config::logLevel(void) {
  return globalLogLevel;
}

bool config::logColored(void) {
  return globalLogColored;
}

Config::Config(const char *File)
  : _scan(File) {
  _it = firstToken();
  while (true)
    _websites.push_back(server());
}

Website Config::server(void) {
  if (match
}

bool Config::sep(void) {
  return newline() || wschar();
}

bool Config::wschar(void) {
  return 
}
