#pragma once

#include "Token.hpp"
#include <list>
#include <stdexcept>
#include <string>

class Scanner {
public:
  explicit Scanner(const char *File);
  ~Scanner(void);
  const std::list<Token> &getTokens(void) const;

  std::list<Token>::const_iterator firstToken(void) const;

private:
  std::list<Token> _tokens;
  void addEof(const size_t Line);
  void addNewline(const size_t Line);
  void scanLine(const size_t Number, const std::string &Str);
};

std::ostream &operator<<(std::ostream &Os, const Scanner &Scan);
