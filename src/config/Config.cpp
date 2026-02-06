#include "Config.hpp"

#include "ConfigDefaults.hpp"
#include "Logging.hpp"
#include <string>

namespace {

/// \var log::t_log_level LogLevel
/// \brief defines the lowest log severity that is still printed
log::LogLevel GlobalLogLevel = DEFAULT_LOG_LEVEL;

} // namespace

log::LogLevel config::getLogLevel(void) { return GlobalLogLevel; }

void config::fromFile(const std::string &File) { (void)File; }
