#pragma once

#include <string>
#include "Logging.hpp"

namespace Config
{
	Log::t_log_level getLogLevel();

	/**
	 * @fn static void fromFile(const std::string& file)
	 *
	 * @brief reads configuration from a file into memory
	 *
	 * @param file the configuration file to read from
	 */
	void fromFile(const std::string& file);
}
