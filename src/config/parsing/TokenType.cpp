#include "TokenType.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

static bool isSingleChar(const char Ch, std::string::const_iterator &It);
static bool isCharset(const char *Charset, const std::string &Str,
                      std::string::const_iterator &It);
static bool isKeyword(const std::string &Keyword, const std::string &Str,
                      std::string::const_iterator &It);

static const TokenType globalTokenTypes[] = {
    {"Name", "", "abcdefghijklmnopqrstuvwxyz", TokenType::Name, TokenType::Charset, 0},
    {"Number", "", "0123456789", TokenType::Number, TokenType::Charset, 0},
    {"Semicolon", "", NULL, TokenType::Semicolon, TokenType::SingleChar, ';'},
    {"BracesLeft", "", NULL, TokenType::BracesLeft, TokenType::SingleChar, '{'},
    {"BracesRight", "", NULL, TokenType::BracesRight, TokenType::SingleChar,
     '}'},
    {"Server", "server", NULL, TokenType::Server, TokenType::Keyword, 0},
    {"Whitespace", "", " \t", TokenType::Whitespace, TokenType::Charset, 0},
    {"Eof", "", NULL, TokenType::Eof, TokenType::CategoryEof, 0}};

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
    return isSingleChar(singleChar, ItNew);
  case Charset:
    return isCharset(charset, Str, ItNew);
  case Keyword:
    return isKeyword(keyword, Str, ItNew);
  case CategoryEof:
    return false;
  }
  return false;
}

static bool isSingleChar(const char Ch, std::string::const_iterator &It) {
  if (*It != Ch)
    return false;
  ++It;
  return true;
}

static bool isCharset(const char *Charset, const std::string &Str,
                      std::string::const_iterator &It) {
  if (!strchr(Charset, *It))
    return false;
  // size_t i = 0;
  // std::string::const_iterator itDup = It;
  // while (i != globalTokenTypesSize) {
  //   if (globalTokenTypes[i].category == TokenType::Keyword &&
  //     globalTokenTypes[i].matchType(Str, It, itDup)) {
  //     return false;
  //   }
  //   ++i;
  // }
  while (It != Str.end() && strchr(Charset, *It))
    ++It;
  return true;
}

static bool isKeyword(const std::string &Keyword, const std::string &Str,
                      std::string::const_iterator &It) {
  if (Keyword.length() > Str.end() - It)
    return false;
  if (Str.compare(It - Str.begin(), Keyword.length(), Keyword))
    return false;
  It += Keyword.length();
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
