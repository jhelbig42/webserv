#include "Parser.hpp"

#include "Config.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include <list>
#include <string>

// class Config
Parser::Parser(Config &Conf, const char *File) : _config(Conf), _scan(File), _line(1) {
}

void Parser::parse(void) {
  _it = _scan.firstToken();
  while (true) {
    skipSep();
    if (match(TokenType::Eof))
      break;
    try {
      _config.addWebsite(server());
    } catch (const UnexpectedTokenException &e) {
      throw;
    } catch (...) {
      throw;
    }
  }
}

Parser::~Parser() { }

const Token &Parser::peek(void) const {
  return *_it;
}

void Parser::eat(void) {
  ++_it;
}

bool Parser::sep(void) {
  return match(TokenType::Newline) || match(TokenType::Whitespace);
}

void Parser::skipSep(void) {
  while (sep())
    ;
}

TokenType::Type Parser::nextType(void) const {
  return peek().getType().type;
}

bool Parser::match(const TokenType::Type Type) {
  if (_it->getType().type == Type) {
    ++_it;
    if (Type == TokenType::Newline)
      ++_line;
    return true;
  }
  return false;
}

bool Parser::noMatch(const TokenType::Type Type) {
  if (_it->getType().type != Type) {
    ++_it;
    if (Type == TokenType::Newline)
      ++_line;
    return true;
  }
  return false;
}

void Parser::throwTokenError(void) {
  throw UnexpectedTokenException(_it);
}

const std::string &Parser::matchGetLexeme(TokenType::Type Type) {
  const std::list<Token>::const_iterator itDup = _it;
  if (!match(Type))
    throwTokenError();
  return itDup->getLexeme();
}
