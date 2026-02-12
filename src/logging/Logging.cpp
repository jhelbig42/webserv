#include "Logging.hpp"

#include "Config.hpp"
#include "LoggingDefines.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

#define RESET_COLOR "\033[0m"
#define REFERENCE_YEAR 1900

static void logInternal(std::ostream &Os, const std::string &Color,
                        const std::string &Label, const std::string &Msg);
static void printTimestamp(std::ostream &Os);

void logging::logString(const logging::LogLevel Level, const std::string &Msg) {
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
    break;
  case Off:
    ;
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
  const std::time_t timeSinceEpoch = std::time(NULL);
  if (timeSinceEpoch == (time_t)(-1))
    return;
  const std::tm *currentTime = std::localtime(&timeSinceEpoch);
  if (currentTime == NULL)
    return;
  Os << currentTime->tm_year + REFERENCE_YEAR;
  Os << '-' << std::setfill('0') << std::setw(2) << currentTime->tm_mon + 1;
  Os << '-' << std::setfill('0') << std::setw(2) << currentTime->tm_mday;
  Os << ' ' << std::setfill('0') << std::setw(2) << currentTime->tm_hour;
  Os << ':' << std::setfill('0') << std::setw(2) << currentTime->tm_min;
  Os << ':' << std::setfill('0') << std::setw(2) << currentTime->tm_sec;
}
