#pragma once

#include <stdexcept>
#include <string>

struct TokenType {
  typedef enum {
    Unknown,
    Eof,
    Newline,
    Semicolon,
    BracesLeft,
    BracesRight,
    MaxRequestBody,
    Dot,
    Slash,
    Asterisk,
    Colon,
    Server,
    Whitespace,
    Autoindex,
    Allow,
    HeadToken,
    PostToken,
    GetToken,
    DeleteToken,
    On,
    Off,
    ErrorPage,
    Location,
    Redirect,
    Cgi,
    Return,
    Listen,
    Number,
    Root,
    Name
  } Type;

  typedef enum { Special, SingleChar, Charset, Keyword, UnknownCat } Category;

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

  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  const std::string identifier;
  const std::string tokenStr;
  const Type type;
  const Category category;
  // NOLINTEND(misc-non-private-member-variables-in-classes)
};
