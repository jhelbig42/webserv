#pragma once

#include "TokenType.hpp"
#include <cstddef>

class Token {
public:
  Token(const size_t Line, const size_t Num, const std::string &Str,
        const std::string::const_iterator It,
        std::string::const_iterator &ItNew);
  Token(const size_t Line, const size_t Num, const TokenType::Type Type);
  const TokenType &getType(void) const;
  const std::string &getLexeme(void) const;
  size_t getLine(void) const;
  size_t getNum(void) const;

private:
  const size_t _line;
  const size_t _num;
  const TokenType &_type;
  std::string _lexeme;
};

std::ostream &operator<<(std::ostream &Os, const Token &Tkn);
