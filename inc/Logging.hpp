#pragma once

#include <string>

class Log
{
	public:

		typedef enum log_level
		{
			DEBUG,
			LOG,
			WARNING,
			ERROR
		} t_log_level;

		/**
		 * @fn static void log(const std::string& msg, const t_log_level level)
		 * 
		 * @brief logs string if LOGLVL is larger or equal to level
		 *
		 * unchecked runtime errors:
		 * level > ERROR || level < DEBUG
		 *
		 * @param msg Message to be logged
		 * @param level minimum LOGLVL to log this message
		 */
		static void log(const std::string& msg, const t_log_level level);
};
