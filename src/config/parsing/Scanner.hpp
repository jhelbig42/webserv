#pragma once

#include <list>
#include <string>

struct TokenType {
  typedef enum {
    Eof,
    Semicolon,
    BracesLeft,
    BracesRight,
    Whitespace,
    Name
  } Type;
  Type type;
  std::string identifier;
  bool (*isType)(const char);
  std::string::const_iterator (*advanceIt)(const std::string &,
                                           const std::string::const_iterator);
  static const TokenType &
  getTokenType(const std::string &Str, const std::string::const_iterator It);
};

class Token {
public:
  Token(const std::string &Str, std::string::const_iterator &It);
  const TokenType &getType(void) const;
  const std::string &getLexeme(void) const;

private:
  const TokenType &_type;
  std::string _lexeme;
};

class Scanner {
public:
  explicit Scanner(const std::string &source);
  ~Scanner();
  std::list<Token *> _tokens;

private:
  const std::string &_source;
};

std::ostream &operator<<(std::ostream &Os, const Token &Tkn);
std::ostream &operator<<(std::ostream &Os, const Scanner &Scan);
