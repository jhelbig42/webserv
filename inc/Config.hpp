#pragma once

#include "Logging.hpp"
#include "Scanner.hpp"
#include "TokenType.hpp"
#include "Website.hpp"
#include <stdexcept>
#include <string>

namespace config {

logging::LogLevel logLevel(void);
bool logColored(void);

} // namespace config

/// \brief object which holds most of the HTTP configuration
class Config {
public:
  explicit Config(const char *File);
  const std::list<Website> &getWebsites(void) const;
  void addWebsite(const Website &Site);

private:
  std::list<Website> _websites;
};

std::ostream &operator<<(std::ostream &Os, const Config &Conf);
