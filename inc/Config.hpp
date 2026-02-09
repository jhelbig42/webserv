#pragma once

#include "Logging.hpp"
#include <string>

namespace config {

logging::LogLevel getLogLevel();

/// \brief reads configuration from a file into memory
///
/// not implemented yet!!
///
/// \param file the configuration file to read from
void fromFile(const std::string &File);

} // namespace config
