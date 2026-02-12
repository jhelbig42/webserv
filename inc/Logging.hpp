#pragma once

#include <string>
#include <sstream>

namespace logging {

typedef enum { Debug, Info, Warning, Error, Off } LogLevel;

void logString(const LogLevel Level, const std::string &Msg);

/// \brief Msg if config::getLogLevel() returns larger or equal to Level
///
/// unchecked runtime errors:
/// level > Off || level < DEBUG
///
/// \param Level minimum return value of config::getLogLevel() to log this
/// message
/// \param Msg Message to be logged
template <typename T>
void log(const LogLevel Level, const T Msg) {
  std::ostringstream oss;
  oss << Msg;
  logString(Level, oss.str());
}

} // namespace log
