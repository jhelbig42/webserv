#include "Logging.hpp"

#include <string>
#include <iostream>
#include "Config.hpp"

#define DEBUG_STREAM std::cout
#define DEBUG_COLOR ""
#define DEBUG_LABEL "[DEBUG]  "

#define LOG_STREAM std::cout
#define LOG_COLOR ""
#define LOG_LABEL "[LOG]    "

#define WARNING_STREAM std::cerr
#define WARNING_COLOR "\x1B[33m"
#define WARNING_LABEL "[WARNING]"

#define ERROR_STREAM std::cerr
#define ERROR_COLOR "\x1B[31m"
#define ERROR_LABEL "[ERROR]  "

static void log_internal(std::ostream& os, const std::string& color, const std::string& label, const std::string& msg);
static void print_timestamp(std::ostream& os);

void Log::log(const std::string& msg, const Log::t_log_level level)
{
	if (level < Config::getLogLevel())
		return;
	switch (level)
	{
		case DEBUG:
			log_internal(DEBUG_STREAM, DEBUG_COLOR, DEBUG_LABEL, msg);
			break;
		case LOG:
			log_internal(LOG_STREAM, LOG_COLOR, LOG_LABEL, msg);
			break;
		case WARNING:
			log_internal(WARNING_STREAM, WARNING_COLOR, WARNING_LABEL, msg);
			break;
		case ERROR:
			log_internal(ERROR_STREAM, ERROR_COLOR, ERROR_LABEL, msg);
	}
}

static void log_internal(std::ostream& os, const std::string& color, const std::string& label, const std::string& msg)
{
	os << color;
	print_timestamp(os);
	os << ' ' << label << ' ' << msg << "\033[0m" << std::endl;
}

static void print_timestamp(std::ostream& os)
{
	os << "2026-02-03 13:37:11";
}
