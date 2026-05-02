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
  size_t _numLines;
  size_t _numTokens;
  std::list<Token> _tokens;
  void addEof(void);
  void addNewline(void);
  void scanLine(const std::string &Str);
};

std::ostream &operator<<(std::ostream &Os, const Scanner &Scan);
