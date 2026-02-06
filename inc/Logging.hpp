#pragma once

#include <string>

namespace Log {
typedef enum log_level { DEBUG, INFO, WARNING, ERROR } t_log_level;

/**
 * @fn void log(const std::string& msg, const t_log_level level)
 *
 * @brief logs string if INFOLVL is larger or equal to level
 *
 * unchecked runtime errors:
 * level > ERROR || level < DEBUG
 *
 * @param msg Message to be logged
 * @param level minimum INFOLVL to log this message
 */
void log(const std::string &msg, const t_log_level level);
} // namespace Log
