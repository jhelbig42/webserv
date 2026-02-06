#include "Config.hpp"

#include "ConfigDefaults.hpp"
#include "Logging.hpp"
#include <string>

namespace {

/// \var Log::t_log_level LogLevel
/// \brief defines the lowest log severity that is still printed
Log::t_log_level LogLevel = DEFAULT_LOG_LEVEL;

} // namespace

Log::t_log_level Config::getLogLevel(void) { return LogLevel; }

void Config::fromFile(const std::string &file) { (void)file; }
