#include "Parser.hpp"

#include "Config.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include <cerrno>
#include <cstdlib>
#include <limits>
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

bool Parser::isNextType(const TokenType::Type Type) const {
  return nextType() == Type;
}

std::string Parser::parseWord(void) {
  std::string word("");
  while (nextType() != TokenType::Newline &&
         nextType() != TokenType::Whitespace &&
         nextType() != TokenType::Semicolon) {
    word += peek().getLexeme();
    eat();
  }
  return word;
}

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

// safe if only called after finished construction of the Parser
// NOLINTBEGIN(bugprone-exception-copy-constructor-throws)
void Parser::throwTokenError(const std::string &Msg) {
  throw UnexpectedTokenException(_it, Msg);
}
// NOLINTEND(bugprone-exception-copy-constructor-throws)

const std::string &Parser::matchGetLexeme(TokenType::Type Type) {
  const std::list<Token>::const_iterator itDup = _it;
  if (!match(Type))
    throwTokenError("");
  return itDup->getLexeme();
}

unsigned int Parser::parseUnsignedInt(void) {
  if (!isNextType(TokenType::Number))
    throwTokenError("expected a number");
  errno = 0;
  const unsigned long code = strtoul(peek().getLexeme().c_str(), NULL, 0);
  if (errno == ERANGE || code > std::numeric_limits<unsigned int>::max())
    throwTokenError("number not in valid range");
  eat();
  return static_cast<unsigned int>(code); // safe becuase of prior check
}
