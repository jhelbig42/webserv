#include "TokenType.hpp"

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>

static bool isSingleChar(const std::string &Ch, std::string::const_iterator &It);
static bool isCharset(const std::string &Charset, const std::string &Str,
                      std::string::const_iterator &It);
static bool isKeyword(const std::string &Keyword, const std::string &Str,
                      std::string::const_iterator &It);
static bool isReserved(const std::string &Word);

// highest priority classification should always come first
// i.e. usually TokenTypes of category TokenType::Charset
static const TokenType globalTokenTypes[] = {
    {"Name", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
     TokenType::Name, TokenType::Charset},
    {"Number", "0123456789", TokenType::Number, TokenType::Charset},
    {"Semicolon", ";", TokenType::Semicolon, TokenType::SingleChar},
    {"Asterisk", "*", TokenType::Asterisk, TokenType::SingleChar},
    {"Dot", ".", TokenType::Dot, TokenType::SingleChar},
    {"BracesLeft", "{", TokenType::BracesLeft, TokenType::SingleChar},
    {"BracesRight", "}", TokenType::BracesRight, TokenType::SingleChar},
    {"Slash", "/", TokenType::Slash, TokenType::SingleChar},
    {"Colon", ":", TokenType::Colon, TokenType::SingleChar},
    {"Server", "server", TokenType::Server, TokenType::Keyword},
    {"Listen", "listen", TokenType::Listen, TokenType::Keyword},
    {"On", "on", TokenType::On, TokenType::Keyword},
    {"Off", "off", TokenType::Off, TokenType::Keyword},
    {"ErrorPage", "error", TokenType::ErrorPage, TokenType::Keyword},
    {"Location", "location", TokenType::Location, TokenType::Keyword},
    {"Return", "return", TokenType::Return, TokenType::Keyword},
    {"Redirect", "redirect", TokenType::Redirect, TokenType::Keyword},
    {"Cgi", "cgi", TokenType::Cgi, TokenType::Keyword},
    {"Root", "root", TokenType::Root, TokenType::Keyword},
    {"Autoindex", "autoindex", TokenType::Autoindex, TokenType::Keyword},
    {"Allow", "allow", TokenType::Allow, TokenType::Keyword},
    {"Head", "HEAD", TokenType::Head, TokenType::Keyword},
    {"Post", "POST", TokenType::Post, TokenType::Keyword},
    {"Get", "GET", TokenType::Get, TokenType::Keyword},
    {"Delete", "DELETE", TokenType::Delete, TokenType::Keyword},
    {"Whitespace", " \t", TokenType::Whitespace, TokenType::Charset},
    {"Eof", "", TokenType::Eof, TokenType::Special},
    {"Newline", "", TokenType::Newline, TokenType::Special},
    {"Unknown", "", TokenType::Unknown, TokenType::UnknownCat}};

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
  return false;
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
  ++ItNew;
  return globalTokenTypes[globalTokenTypesSize - 1];
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
  case UnknownCat:
    return false;
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
  const std::string::const_iterator itDup = It;
  while (It != Str.end() && strchr(Charset.c_str(), *It))
    ++It;
  const std::string word(itDup, It);
  if (isReserved(word))
    return false;
  return true;
}

static bool isReserved(const std::string &Word) {
  for (size_t i = 0; i != globalTokenTypesSize; ++i) {
    if (globalTokenTypes[i].category == TokenType::Keyword && Word == globalTokenTypes[i].tokenStr)
      return true;
  }
  return false;
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

TokenType::UnrecognizedTokenException::UnrecognizedTokenException(const std::string &Str)
    : std::runtime_error(Str) {
}

