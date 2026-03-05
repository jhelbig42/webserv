#pragma once

#include "Logging.hpp"
#include "TokenType.hpp"
#include "Website.hpp"
#include "Scanner.hpp"
#include <string>

namespace config {

logging::LogLevel logLevel(void);
bool logColored(void);

} // namespace config

class Config {
public:
  Config(const char *File);
  const std::list<Website> &getWebsites(void) const;

private:
  void addEntry(Website &site);
  void addIpv4(Listen &interface);
  const std::string &matchGetLexeme(TokenType::Type Type);
  void addPort(Listen &interface);
  bool sep(void);
  void skipSep(void);
  bool match(TokenType::Type Type);
  Website expression(void);
  Website server(void);
  Scanner _scan;
  std::list<Website> _websites;
  std::list<Token>::const_iterator _it;
};

std::ostream &operator<<(std::ostream &Os, const Config &Conf);
