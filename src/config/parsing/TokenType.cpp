#include "Scanner.hpp"
#include <cstring>
#include <stdexcept>

bool isEof(const char Ch);
bool isSemicolon(const char Ch);
bool isBracesLeft(const char Ch);
bool isBracesRight(const char Ch);
bool isWhitespace(const char Ch);
bool isName(const char Ch);

std::string::const_iterator advanceEof(const std::string &Str,
                                       const std::string::const_iterator It);
std::string::const_iterator
advanceSemicolon(const std::string &Str, const std::string::const_iterator It);
std::string::const_iterator
advanceBracesLeft(const std::string &Str, const std::string::const_iterator It);
std::string::const_iterator
advanceBracesRight(const std::string &Str,
                   const std::string::const_iterator It);
std::string::const_iterator
advanceWhitespace(const std::string &Str, const std::string::const_iterator It);
std::string::const_iterator advanceName(const std::string &Str,
                                        const std::string::const_iterator It);

static const TokenType globalTokenTypes[] = {
    {TokenType::Eof, "Eof", &isEof, &advanceEof},
    {TokenType::Semicolon, "Semicolon", &isSemicolon, &advanceSemicolon},
    {TokenType::BracesLeft, "BracesLeft", &isBracesLeft, &advanceBracesLeft},
    {TokenType::BracesRight, "BracesRight", &isBracesRight,
     &advanceBracesRight},
    {TokenType::Whitespace, "Whitespace", &isWhitespace, &advanceWhitespace},
    {TokenType::Name, "Name", &isName, &advanceName}};

static const size_t globalTokenTypesSize =
    sizeof(globalTokenTypes) / sizeof(*globalTokenTypes);

bool isEof(const char Ch) {
  return false;
}

bool isSemicolon(const char Ch) {
  return Ch == ';';
}

bool isBracesLeft(const char Ch) {
  return Ch == '{';
}

bool isBracesRight(const char Ch) {
  return Ch == '}';
}

bool isWhitespace(const char Ch) {
  return isspace(Ch);
}

bool isName(const char Ch) {
  static const char nameChar[] = "abcdefghijklmnopqrstuvwxyz";
  return strchr(nameChar, Ch);
}

std::string::const_iterator advanceEof(const std::string &Str,
                                       const std::string::const_iterator It) {
  (void)Str;
  return It;
}

std::string::const_iterator
advanceSemicolon(const std::string &Str, const std::string::const_iterator It) {
  (void)Str;
  return It + 1;
}

std::string::const_iterator
advanceBracesLeft(const std::string &Str,
                  const std::string::const_iterator It) {
  (void)Str;
  return It + 1;
}

std::string::const_iterator
advanceBracesRight(const std::string &Str,
                   const std::string::const_iterator It) {
  (void)Str;
  return It + 1;
}

std::string::const_iterator
advanceWhitespace(const std::string &Str,
                  const std::string::const_iterator It) {
  std::string::const_iterator ItDup = It;
  while (ItDup != Str.end() && isWhitespace(*ItDup))
    ++ItDup;
  return ItDup;
}

std::string::const_iterator advanceName(const std::string &Str,
                                        const std::string::const_iterator It) {
  std::string::const_iterator ItDup = It;
  while (ItDup != Str.end() && isName(*ItDup))
    ++ItDup;
  return ItDup;
}

const TokenType &TokenType::getTokenType(const std::string &Str,
                                         const std::string::const_iterator It) {
  for (size_t i = 0; i != globalTokenTypesSize; ++i) {
    if (globalTokenTypes[i].isType(*It)) {
      return globalTokenTypes[i];
    }
  }
  throw std::runtime_error("unrecognized token");
}

const TokenType &TokenType::getTokenType2(const TokenType::Type Type) {
  for (size_t i = 0; i != globalTokenTypesSize; ++i) {
    if (globalTokenTypes[i].type == Type) {
      return globalTokenTypes[i];
    }
  }
  throw std::runtime_error("unrecognized token");
}
