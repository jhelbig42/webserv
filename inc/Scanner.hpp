#pragma once

#include "Token.hpp"
#include <list>
#include <string>

class Scanner {
public:
  explicit Scanner(const char *File);
  ~Scanner(void);
  const std::list<const Token *> &getTokens(void) const;

private:
  std::list<const Token *> _tokens;
  void addEof(const size_t Line);
  void addNewline(const size_t Line);
  void scanLine(const size_t Number, const std::string &Str);
};

std::ostream &operator<<(std::ostream &Os, const Scanner &Scan);
