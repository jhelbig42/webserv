#include "Config.hpp"
#include "ConfigDefaults.hpp"
#include "Logging.hpp"

/// \var static logging::LogLevel globalLogLevel
/// \brief defines the lowest log severity that is still printed
static logging::LogLevel globalLogLevel = LOG_LEVEL;

/// \var static bool globalLogColored
/// \brief defines if log messages are colored
static bool globalLogColored = LOG_COLORED;

// global configs
logging::LogLevel config::logLevel(void) {
  return globalLogLevel;
}

bool config::logColored(void) {
  return globalLogColored;
}
