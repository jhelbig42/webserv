#include "Config.hpp"

#include "ConfigDefaults.hpp"
#include "Logging.hpp"
#include <string>

/// \var static logging::LogLevel globalLogLevel
/// \brief defines the lowest log severity that is still printed
static logging::LogLevel globalLogLevel = LOG_LEVEL;

/// \var static bool globalLogColored
/// \brief defines if log messages are colored
static bool globalLogColored = LOG_COLORED;

logging::LogLevel config::logLevel(void) {
  return globalLogLevel;
}

bool config::logColored(void) {
  return globalLogColored;
}

Config::Config(const char *File) : _scan(File) {
  _it = _scan.firstToken();
  while (true) {
    while (sep())
      ;
    _websites.push_back(server());
  }
}

Website Config::server(void) {
  if (!match(TokenType::Server))
    throw std::runtime_error("unexpected token");
  while (sep())
    ;
  if (!match(TokenType::BracesLeft))
    throw std::runtime_error("unexpected token");
  Website newWebsite;
  while (!match(TokenType::BracesRight)) {
    skipSep();
    addEntry(newWebsite);
    skipSep();
    if (!match(TokenType::Semicolon))
      throw std::runtime_error("unexpected token");
    skipSep();
  }
  return newWebsite;
}

bool Config::sep(void) {
  return match(TokenType::Newline) || match(TokenType::Whitespace);
}

void Config::skipSep(void) {
  while (sep())
    ;
}

bool Config::match(TokenType::Type Type) {
  if (_it->getType().type == Type) {
    ++_it;
    return true;
  }
  return false;
}

void Config::addEntry(Website &site) {
  Listen interface;
  if (match(TokenType::Listen)) {
    if (!sep())
      throw std::runtime_error("unexpected token");
    skipSep();
    addIpv4(interface);
    if (!match(TokenType::Colon))
      throw std::runtime_error("unexpected token");
    addPort(interface);
    site.addInterface(interface);
  }
  throw std::runtime_error("invalid entry");
}

void Config::addIpv4(Listen &interface) {
  try {
    interface.ip = matchGetLexeme(TokenType::Number);
    interface.ip += matchGetLexeme(TokenType::Dot);
    interface.ip += matchGetLexeme(TokenType::Number);
    interface.ip += matchGetLexeme(TokenType::Dot);
    interface.ip += matchGetLexeme(TokenType::Number);
    interface.ip += matchGetLexeme(TokenType::Dot);
    interface.ip += matchGetLexeme(TokenType::Number);
  } catch (...) {
    throw std::runtime_error("invalid ipv4");
  }
}

const std::string &Config::matchGetLexeme(TokenType::Type Type) {
  std::string::const_iterator itDup = _it;
  if (!match(Type))
    throw std::runtime_error("unexpected token");
  return itDup->_lexeme;
}

void Config::addPort(Listen &interface) {
  try {
    interface.ip = matchGetLexeme(TokenType::Number);
  } catch (...) {
    throw std::runtime_error("invalid ipv4");
  }
}

std::ostream &operator<<(std::ostream &Os, const Config &Conf) {
  std::list<Website>::const_iterator it = Conf.getWebsites().begin();
  while (it != Conf.getWebsites().end()) {
    Os << *it << '\n';
  }
}

const std::list<Website> &getWebsites(void) const {
  return _websites;
}
