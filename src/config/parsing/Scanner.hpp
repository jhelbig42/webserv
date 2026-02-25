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
    Number,
    Name
  } Type;
  Type type;
  std::string identifier;
  bool (*isType)(const char);
  std::string::const_iterator (*advanceIt)(const std::string &,
                                           const std::string::const_iterator);
  static const TokenType &getTokenType(const std::string &Str,
                                       const std::string::const_iterator It);
  static const TokenType &getTokenType2(const Type Type);
};

class Token {
public:
  Token(const size_t Line, const std::string &Str,
        std::string::const_iterator &It);
  Token(const size_t Line, const TokenType::Type Type);
  const TokenType &getType(void) const;
  const std::string &getLexeme(void) const;
  size_t getLine(void) const;

private:
  const size_t _line;
  const TokenType &_type;
  std::string _lexeme;
};

class Scanner {
public:
  explicit Scanner(const char *File);
  ~Scanner(void);
  std::list<Token *> _tokens;
  void addEof(const size_t Line);

private:
  void scanLine(const size_t Number, const std::string &Str);
};

std::ostream &operator<<(std::ostream &Os, const Token &Tkn);
std::ostream &operator<<(std::ostream &Os, const Scanner &Scan);
