#include "Config.hpp"

#include "ConfigDefaults.hpp"
#include "Logging.hpp"
#include <string>

/// \var static logging::LogLevel GlobalLogLevel
/// \brief defines the lowest log severity that is still printed
static logging::LogLevel globalLogLevel = DEFAULT_LOG_LEVEL;

logging::LogLevel config::getLogLevel(void) {
  return globalLogLevel;
}

void config::fromFile(const std::string &File) {
  (void)File;
}
