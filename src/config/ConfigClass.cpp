#include "Config.hpp"
#include "ConfigDefaults.hpp"
#include "Logging.hpp"
#include <string>
#include <iostream>

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

const Token &Config::peek(void) const {
  return *_it;
}

void Config::eat(void) {
  ++_it;
}

Website Config::server(void) {
  if (!match(TokenType::Server))
    throwTokenError();
  while (sep())
    ;
  if (!match(TokenType::BracesLeft))
    throwTokenError();
  Website newWebsite;
  while (!match(TokenType::BracesRight)) {
    skipSep();
    addEntry(newWebsite);
    skipSep();
    if (!match(TokenType::Semicolon))
      throwTokenError();
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
    if (Type == TokenType::Newline)
      ++_line;
    return true;
  }
  return false;
}

bool Config::noMatch(TokenType::Type Type) {
  if (_it->getType().type != Type) {
    ++_it;
    if (Type == TokenType::Newline)
      ++_line;
    return true;
  }
  return false;
}

void Config::addEntry(Website &site) {
  if (match(TokenType::Listen)) {
    if (!sep())
      throwTokenError();
    skipSep();
    Listen interface;
    addIpv4(interface);
    if (!match(TokenType::Colon))
      throwTokenError();
    addPort(interface);
    site.addInterface(interface);
  } else if (match(TokenType::Root)) {
    if (!sep())
      throwTokenError();
    skipSep();
    const std::string root = parseAbsPath();
    site.setRoot(root);
  }
  else
    throwTokenError();
}

void Config::throwTokenError(void) {
  std::ostringstream oss;
  oss << "Unexpected token in line " << _line << ": `" << _it->getLexeme() << '\'';
  throw UnexpectedTokenException(oss.str());
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
    throwTokenError();
  }
}

std::string Config::parseAbsPath(void) {
  std::string path = matchGetLexeme(TokenType::Slash);
  if (sep() || peek().getType().type == TokenType::Semicolon)
    return path;
  while (true) {
    if (peek().getType().type == TokenType::Slash)
      throwTokenError();
    while (peek().getType().type != TokenType::Newline
           && peek().getType().type != TokenType::Whitespace
           && peek().getType().type != TokenType::Semicolon
           && peek().getType().type != TokenType::Slash) {
      path += peek().getLexeme();
      eat();
    }
    if (!match(TokenType::Slash))
      throwTokenError();
    path += "/";
    if (sep() || peek().getType().type == TokenType::Semicolon)
      return path;
  }
}

const std::string &Config::matchGetLexeme(TokenType::Type Type) {
  std::list<Token>::const_iterator itDup = _it;
  if (!match(Type))
    throwTokenError();
  return itDup->getLexeme();
}

void Config::addPort(Listen &interface) {
  try {
    interface.port = matchGetLexeme(TokenType::Number);
  } catch (...) {
    throwTokenError();
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
