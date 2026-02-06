#pragma once

#include "Logging.hpp"
#include <string>

namespace Config {

Log::t_log_level getLogLevel();

/// \fn static void fromFile(const std::string& file)
///
/// \brief reads configuration from a file into memory
///
/// not implemented yet!!
///
/// \param file the configuration file to read from
void fromFile(const std::string &file);

} // namespace Config
