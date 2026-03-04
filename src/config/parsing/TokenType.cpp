#include "TokenType.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

static bool isSingleChar(const std::string &Ch, std::string::const_iterator &It);
static bool isCharset(const std::string &Charset, const std::string &Str,
                      std::string::const_iterator &It);
static bool isKeyword(const std::string &Keyword, const std::string &Str,
                      std::string::const_iterator &It);

// highest priority classification should always come first
// i.e. usually TokenTypes of category TokenType::Charset
static const TokenType globalTokenTypes[] = {
    {"Name", "abcdefghijklmnopqrstuvwxyz", TokenType::Name,
     TokenType::Charset},
    {"Number", "0123456789", TokenType::Number, TokenType::Charset},
    {"Semicolon", ";", TokenType::Semicolon, TokenType::SingleChar},
    {"BracesLeft", "{", TokenType::BracesLeft, TokenType::SingleChar},
    {"BracesRight", "}", TokenType::BracesRight, TokenType::SingleChar},
    {"Server", "server", TokenType::Server, TokenType::Keyword},
    {"Whitespace", " \t", TokenType::Whitespace, TokenType::Charset},
    {"Eof", "", TokenType::Eof, TokenType::Special},
    {"Newline", "", TokenType::Newline, TokenType::Special}};

static const size_t globalTokenTypesSize =
    sizeof(globalTokenTypes) / sizeof(*globalTokenTypes);

bool TokenType::isType(const Type Type, const std::string &Str,
                       std::string::const_iterator It) {
  size_t i = 0;
  std::string::const_iterator dummy;
  while (i != globalTokenTypesSize) {
    if (globalTokenTypes[i].type == Type)
      return globalTokenTypes[i].matchType(Str, It, dummy);
    ++i;
  }
  throw std::runtime_error("requested unlisted token type");
}

const TokenType &TokenType::getTokenType(const std::string &Str,
                                         const std::string::const_iterator It,
                                         std::string::const_iterator &ItNew) {
  size_t i = 0;
  while (i != globalTokenTypesSize) {
    if (globalTokenTypes[i].matchType(Str, It, ItNew))
      return globalTokenTypes[i];
    ++i;
  }
  throw std::runtime_error("unrecognized token");
}

bool TokenType::matchType(const std::string &Str,
                          const std::string::const_iterator It,
                          std::string::const_iterator &ItNew) const {
  ItNew = It;
  switch (category) {
  case SingleChar:
    return isSingleChar(tokenStr, ItNew);
  case Charset:
    return isCharset(tokenStr, Str, ItNew);
  case Keyword:
    return isKeyword(tokenStr, Str, ItNew);
  case Special:
    return false;
  }
  return false;
}

static bool isSingleChar(const std::string &Ch, std::string::const_iterator &It) {
  if (*It != Ch[0])
    return false;
  ++It;
  return true;
}

static bool isCharset(const std::string &Charset, const std::string &Str,
                      std::string::const_iterator &It) {
  if (!strchr(Charset.c_str(), *It))
    return false;
  while (It != Str.end() && strchr(Charset.c_str(), *It))
    ++It;
  return true;
}

static bool isKeyword(const std::string &Keyword, const std::string &Str,
                      std::string::const_iterator &It) {
  if (Keyword.length() > static_cast<std::string::size_type>(Str.end() - It))
    return false;
  if (Str.compare(static_cast<std::string::size_type>(It - Str.begin()),
                  Keyword.length(), Keyword))
    return false;
  It += static_cast<std::string::difference_type>(Keyword.length());
  return true;
}

const TokenType &TokenType::getTokenTypeId(const TokenType::Type Id) {
  size_t i = 0;
  while (i != globalTokenTypesSize) {
    if (globalTokenTypes[i].type == Id)
      return globalTokenTypes[i];
    ++i;
  }
  throw std::runtime_error("unrecognized token");
}
