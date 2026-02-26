#pragma once

#include <string>

struct TokenType {
  typedef enum {
    Eof,
    Semicolon,
    BracesLeft,
    BracesRight,
    Dot,
    Asterisk,
    Server,
    Whitespace,
    Number,
    Name
  } Type;

  typedef enum { CategoryEof, SingleChar, Charset, Keyword } Category;

  const std::string identifier;
  const std::string keyword;
  const char *charset;
  const Type type;
  const Category category;
  const char singleChar;

  bool matchType(const std::string &Str, const std::string::const_iterator It,
                 std::string::const_iterator &ItNew) const;

  static const TokenType &getTokenType(const std::string &Str,
                                       const std::string::const_iterator It,
                                       std::string::const_iterator &ItNew);

  static const TokenType &getTokenTypeId(const Type Id);

  static bool isType(const Type Type, const std::string &Str,
                     std::string::const_iterator It);
};
