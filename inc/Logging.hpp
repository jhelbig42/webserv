#pragma once

#include <sstream>
#include <string>

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
template <typename T> void log(const LogLevel Level, const T Msg1) {
  std::ostringstream oss;
  oss << Msg1;
  logString(Level, oss.str());
}

template <typename T, typename U>
void log2(const LogLevel Level, const T Msg1, const U Msg2) {
  std::ostringstream oss;
  oss << Msg1 << Msg2;
  logString(Level, oss.str());
}

template <typename T, typename U, typename V>
void log3(const LogLevel Level, const T Msg1, const U Msg2, const V Msg3) {
  std::ostringstream oss;
  oss << Msg1 << Msg2 << Msg3;
  logString(Level, oss.str());
}

} // namespace logging
