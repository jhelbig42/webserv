#include "Config.hpp"

#include <string>
#include "Logging.hpp"
#include "Defaults.hpp"

/**
 * @var static Log::t_log_level LogLevel
 * @brief defines the lowest log severity that is still printed 
 */
static Log::t_log_level LogLevel = DEFAULT_LOG_LEVEL;

Log::t_log_level Config::getLogLevel(void)
{
	return LogLevel;
}

void Config::fromFile(const std::string& file)
{
	(void)file;
}
