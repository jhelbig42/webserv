#pragma once

#include <stdexcept>
#include <string>

struct TokenType {
  typedef enum {
    Eof,
    Newline,
    Semicolon,
    BracesLeft,
    BracesRight,
    Dot,
    Slash,
    Asterisk,
    Colon,
    Server,
    Whitespace,
    Listen,
    Number,
    Root,
    Name
  } Type;

  typedef enum { Special, SingleChar, Charset, Keyword } Category;

  bool matchType(const std::string &Str, const std::string::const_iterator It,
                 std::string::const_iterator &ItNew) const;

  static const TokenType &getTokenType(const std::string &Str,
                                       const std::string::const_iterator It,
                                       std::string::const_iterator &ItNew);

  static const TokenType &getTokenTypeId(const Type Id);

  static bool isType(const Type Type, const std::string &Str,
                     std::string::const_iterator It);

  class UnrecognizedTokenException : public std::runtime_error {
  public:
    explicit UnrecognizedTokenException(const std::string &);
  };

  const std::string identifier;
  const std::string tokenStr;
  const Type type;
  const Category category;
};
