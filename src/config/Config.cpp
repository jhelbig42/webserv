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
  _it = firstToken();
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
  return match(TokenType::Newline || match(TokenType::Whitespace));
}

void Config::skipSep(void) {
  while (sep())
    ;
}

bool Config::match(TokenType::Type Type) {
  if (_it->_type.type == Type) {
    ++_it;
    return true;
  }
  return false;
}

void Config::addEntry(Website &site) {
  if (match(TokenType::Listen)) {
    if (!sep())
      throw std::runtime_error("unexpected token");
    skipSep();
    addIp(site);
    if (!match(TokenType::Colon))
      throw std::runtime_error("unexpected token");
    addPort(site);
  }
  throw std::runtime_error("invalid entry");
}
