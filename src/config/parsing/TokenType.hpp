#pragma once

struct TokenType {
  typedef enum {
    Eof,
    Semicolon,
    BracesLeft,
    BracesRight,
    Dot,
    Asterisk,
    Whitespace,
    Number,
    Name
  } Type;
  typedef enum {
    Eof,
    SingleChar,
    Charset,
    Keyword
  } Category;
  const std::string identifier;
  const std::string keyword;
  const char *charset;
  const Type type;
  const Category category;
  const char singleChar;

  static bool isType(Type Type, const std::string &Str, std::string::const_iterator &It);
  static const TokenType &getTokenType(const std::string &Str, const std::string::const_iterator It);
  static const TokenType &getTokenType2(const Type Type);
};
