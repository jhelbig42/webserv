#include "Config.hpp"

#include "ConfigDefaults.hpp"
#include "Logging.hpp"
#include <string>

/// \var static log::LogLevel GlobalLogLevel
/// \brief defines the lowest log severity that is still printed
static log::LogLevel GlobalLogLevel = DEFAULT_LOG_LEVEL;

log::LogLevel config::getLogLevel(void) { return GlobalLogLevel; }

void config::fromFile(const std::string &File) { (void)File; }
