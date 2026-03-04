#pragma once

#include "Logging.hpp"
#include "TokenType.hpp"
#include <string>

namespace config {

logging::LogLevel logLevel(void);
bool logColored(void);

} // namespace config

class Config {
public:
  Config(const char *File);
  const Website &getWebsites(void) const;

private:
  bool match(TokenType::Type Type);
  Website expression(void);
  Website server(void);
  Scanner _scan;
  std::list<const Website> _websites;
  std::list<Token>::const_iterator _it;
};
