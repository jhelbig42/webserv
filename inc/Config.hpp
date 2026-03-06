#pragma once

#include "Logging.hpp"
#include "TokenType.hpp"
#include "Website.hpp"
#include "Scanner.hpp"
#include <stdexcept>
#include <string>

namespace config {

logging::LogLevel logLevel(void);
bool logColored(void);

} // namespace config

/// \brief object which holds most of the HTTP configuration
class Config {
public:
  Config(const char *File);
  const std::list<Website> &getWebsites(void) const;

  class UnexpectedTokenException : public std::runtime_error {
  public:
    explicit UnexpectedTokenException(const std::string &);
  };

private:
  bool sep(void);
  void skipSep(void);
  bool match(TokenType::Type Type);
  const std::string &matchGetLexeme(TokenType::Type Type);

  Website expression(void);

  Website server(void);

  void addEntry(Website &site);

  void addIpv4(Listen &interface);
  void addPort(Listen &interface);

  void throwTokenError(void);

  Scanner _scan;
  unsigned int _line;
  std::list<Website> _websites;
  std::list<Token>::const_iterator _it;
};

std::ostream &operator<<(std::ostream &Os, const Config &Conf);
