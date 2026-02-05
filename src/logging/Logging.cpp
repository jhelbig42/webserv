#include "Logging.hpp"

#include "Config.hpp"
#include "LoggingDefines.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace {
void log_internal(std::ostream &os, const std::string &color,
                  const std::string &label, const std::string &msg);
void print_timestamp(std::ostream &os);
} // namespace

void Log::log(const std::string &msg, const Log::t_log_level level) {
  if (level < Config::getLogLevel())
    return;
  switch (level) {
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

namespace {
void log_internal(std::ostream &os, const std::string &color,
                  const std::string &label, const std::string &msg) {
  os << color;
  print_timestamp(os);
  os << ' ' << label << ' ' << msg << "\033[0m\n" << std::flush;
}

/**
 * @fn void print_timestamp(std::ostream& os)
 *
 * @brief writes timestamp to output stream
 *
 * TODO: consider removing reduntant stream manipulators
 *
 * @param os output stream to write to
 */
void print_timestamp(std::ostream &os) {
  std::time_t ct_since_epoch = std::time(NULL);
  if (ct_since_epoch == (time_t)(-1))
    return;
  std::tm *ct = std::localtime(&ct_since_epoch);
  if (ct == NULL)
    return;
  os << ct->tm_year + 1900 << '-' << std::setfill('0') << std::setw(2)
     << (ct->tm_mon + 1) << '-' << std::setfill('0') << std::setw(2)
     << ct->tm_mday << ' ' << std::setfill('0') << std::setw(2) << ct->tm_hour
     << ':' << std::setfill('0') << std::setw(2) << ct->tm_min << ':'
     << std::setfill('0') << std::setw(2) << ct->tm_sec;
}
} // namespace
