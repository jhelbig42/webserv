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

void config::fromFile(const std::string &File) {
  (void)File;
}
