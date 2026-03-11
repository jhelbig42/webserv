#include "Config.hpp"

#include "TokenType.hpp"
#include "Website.hpp"
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

Config::UnexpectedTokenException::UnexpectedTokenException(const std::string &str)
    : std::runtime_error(str) {
}

// class Config
Config::Config(const char *File) : _scan(File), _line(1) {
  _it = _scan.firstToken();
  while (true) {
    skipSep();
    if (match(TokenType::Eof))
      break;
    try {
      _websites.push_back(server());
    } catch (const UnexpectedTokenException &e) {
      throw;
    } catch (...) {
      throw;
    }
  }
}

std::ostream &operator<<(std::ostream &Os, const Config &Conf) {
  std::list<Website>::const_iterator it = Conf.getWebsites().begin();
  while (it != Conf.getWebsites().end()) {
    Os << *it++ << '\n';
  }
  return Os;
}

const std::list<Website> &Config::getWebsites(void) const {
  return _websites;
}
