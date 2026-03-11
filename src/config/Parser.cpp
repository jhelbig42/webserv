#include "Config.hpp"

#include "Token.hpp"
#include "TokenType.hpp"
#include <list>
#include <string>

// class Config
Parser::Parser(Config &Conf, const char *File) : _config(Conf), _scan(File), _line(1) {
}

Parser::parse(Config &Conf) {
  _it = _scan.firstToken();
  while (true) {
    skipSep();
    if (match(TokenType::Eof))
      break;
    try {
      Conf.addWebsite(server());
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

bool Config::sep(void) {
  return match(TokenType::Newline) || match(TokenType::Whitespace);
}

void Config::skipSep(void) {
  while (sep())
    ;
}

TokenType::Type Config::nextType(void) const {
  return peek().getType().type;
}

bool Config::match(const TokenType::Type Type) {
  if (_it->getType().type == Type) {
    ++_it;
    if (Type == TokenType::Newline)
      ++_line;
    return true;
  }
  return false;
}

bool Config::noMatch(const TokenType::Type Type) {
  if (_it->getType().type != Type) {
    ++_it;
    if (Type == TokenType::Newline)
      ++_line;
    return true;
  }
  return false;
}

void Config::throwTokenError(void) {
  throw UnexpectedTokenException(_it);
}

const std::string &Config::matchGetLexeme(TokenType::Type Type) {
  const std::list<Token>::const_iterator itDup = _it;
  if (!match(Type))
    throwTokenError();
  return itDup->getLexeme();
}
