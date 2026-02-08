#pragma once

#include <string>

namespace log {

typedef enum { Debug, Info, Warning, Error } LogLevel;

/// \fn void log(const std::string& msg, const t_log_level level)
///
/// \brief logs string if INFOLVL is larger or equal to level
///
/// unchecked runtime errors:
/// level > ERROR || level < DEBUG
///
/// \param msg Message to be logged
/// \param level minimum return value of Config::getLogLevel() to log this
/// message
void log(const std::string &Msg, const LogLevel Level);

} // namespace log
