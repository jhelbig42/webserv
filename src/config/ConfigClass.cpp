#include "Config.hpp"

#include "TokenType.hpp"
#include "Website.hpp"
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

Config::UnexpectedTokenException::UnexpectedTokenException(const std::list<Token>::const_iterator It) { 
  std::list<Token>::const_iterator it = It;
  while (it->getNum() > 1 && it->getLine() == It->getLine())
    --it;
  if (it->getLine() != It->getLine())
    ++it;
  std::string lineStr("");
  size_t linePosition = 0;
  bool reachedPos = false;
  do {
    if (it != It && !reachedPos) {
      lineStr += it->getLexeme();
      linePosition += it->getLexeme().length();
    }
    else if (it == It) {
      lineStr += "\x1B[31m" + it->getLexeme() + "\033[0m";
      reachedPos = true;
    }
    else
      lineStr += it->getLexeme();
    ++it;
  } while (it->getType().type != TokenType::Eof &&
           it->getLine() == It->getLine());
  std::string showStr = lineStr.substr(0, linePosition + 1);
  for (size_t i = 0; i != linePosition; ++i) {
    if (showStr[i] != '\t')
      showStr[i] = '-';
  }
  showStr[linePosition] = '^';
  std::ostringstream oss;
  oss << "Unexpected token in line " << It->getLine() << ":\n";
  oss << lineStr << '\n';
  oss << /*"\x1B[32m" << */showStr << "\033[0m";
  _report = oss.str();
}

const char *Config::UnexpectedTokenException::what() const throw() {
  return _report.c_str();
}

Config::UnexpectedTokenException::~UnexpectedTokenException(void) throw() { }

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
