#include "Config.hpp"

#include "Token.hpp"
#include "TokenType.hpp"
#include <list>
#include <sstream>
#include <string>

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
  std::ostringstream oss;
  oss << "Unexpected token in line " << _line << ": `" << _it->getLexeme() << '\'';
  throw UnexpectedTokenException(oss.str());
}

const std::string &Config::matchGetLexeme(TokenType::Type Type) {
  const std::list<Token>::const_iterator itDup = _it;
  if (!match(Type))
    throwTokenError();
  return itDup->getLexeme();
}
