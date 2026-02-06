#include "Logging.hpp"

#include "Config.hpp"
#include "LoggingDefines.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

#define RESET_COLOR "\033[0m" 

static void logInternal(std::ostream &Os, const std::string &Color,
				const std::string &Label, const std::string &Msg);
static void printTimestamp(std::ostream &Os);

void log::log(const std::string &Msg, const log::LogLevel Level) {
  if (Level < config::getLogLevel())
    return;
  switch (Level) {
  case Debug:
    logInternal(DEBUG_STREAM, DEBUG_COLOR, DEBUG_LABEL, Msg);
    break;
  case Info:
    logInternal(INFO_STREAM, INFO_COLOR, INFO_LABEL, Msg);
    break;
  case Warning:
    logInternal(WARNING_STREAM, WARNING_COLOR, WARNING_LABEL, Msg);
    break;
  case Error:
    logInternal(ERROR_STREAM, ERROR_COLOR, ERROR_LABEL, Msg);
  }
}

static void logInternal(std::ostream &Os, const std::string &Color,
                  const std::string &Label, const std::string &Msg) {
  Os << Color;
  printTimestamp(Os);
  Os << ' ' << Label << ' ' << Msg << RESET_COLOR << '\n' << std::flush;
}

/// \fn static void printTimestamp(std::ostream& os)
/// 
/// \brief writes timestamp to output stream
/// 
/// TODO: consider removing reduntant stream manipulators
/// 
/// \param os output stream to write to
static void printTimestamp(std::ostream &Os) {
  std::time_t ct_since_epoch = std::time(NULL);
  if (ct_since_epoch == (time_t)(-1))
    return;
  std::tm *ct = std::localtime(&ct_since_epoch);
  if (ct == NULL)
    return;
  Os << ct->tm_year + 1900;
  Os << '-' << std::setfill('0') << std::setw(2) << (ct->tm_mon + 1);
  Os << '-' << std::setfill('0') << std::setw(2) << ct->tm_mday;
  Os << ' ' << std::setfill('0') << std::setw(2) << ct->tm_hour;
  Os << ':' << std::setfill('0') << std::setw(2) << ct->tm_min;
  Os << ':' << std::setfill('0') << std::setw(2) << ct->tm_sec;
}
