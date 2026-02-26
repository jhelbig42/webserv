#include "Scanner.hpp"
#include <cstring>
#include <stdexcept>

static const TokenType globalTokenTypes[] = {
    {"Semicolon", NULL, NULL, TokenType::Semicolon, TokenType::SingleChar, ';'},
    {"BracesLeft", NULL, NULL, TokenType::BracesLeft, TokenType::SingleChar, '{'},
    {"BracesRight", NULL, NULL, TokenType::BracesRight, TokenType::SingleChar, '}'},
    {"Server", "server", NULL, TokenType::Server, TokenType::Keyword, 0},
    {"Whitespace", NULL, " \t", TokenType::Whitespace, TokenType::Charset, 0},
    {"Eof", NULL, NULL, TokenType::Eof, TokenType::Eof, 0}}

static const size_t globalTokenTypesSize =
    sizeof(globalTokenTypes) / sizeof(*globalTokenTypes);

static bool isSingleChar(const char Ch, std::string::const_iterator &It);
static bool isCharset(const char *Charset, const std::string &Str, std::string::const_iterator &It);
static bool isKeyword(const std::string &Keyword, const std::string &Str, std::string::const_iterator &It);

static bool isType(Type Type, const std::string &Str, std::string::const_iterator &It) {
  size_t i = 0;
  while (i != globalTokenTypesSize) {
    if (globalTokenTypes[i].type)
      break;
    ++i;
  }
  switch (globalTokenTypes[i].category) {
    case SingleChar:
      return isSingleChar(globalTokenTypes[i].singleChar, It);
    case Charset:
      return isCharset(globalTokenTypes[i].Charset, Str, It);
    case Keyword:
      return isKeyword(globalTokenTypes[i].Keyword, Str, It);
    case Eof:
      return false
  }
}

bool isSingleChar(const char Ch, std::string::const_iterator &It) {
  if (*It != Ch)
    return false;
  ++It;
  return true;
}

bool isCharset(const char *Charset, const std::string &Str, std::string::const_iterator &It) {
  if (!strchr(*It, Charset))
    return false;
  ++It;
  while (It != Str.end() && strchr(*It, Charset))
    ++It;
  return true;
}

bool isKeyword(const std::string &Keyword, const std::string &Str, std::string::const_iterator &It) {
  if (Keyword.length() > Str.end() - It)
    return false;
  if (Str.compare(It - Str.begin(), Keyword.length(), Keyword))
    return false;
  It += Keyword.length();
  return true;
}
