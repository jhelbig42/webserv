#pragma once

#include "TokenType.hpp"
#include <cstddef>

class Token {
public:
  // typedef union {
  //   std::string lexeme;
  //   off_t requestSize;
  //   unsigned char octet;
  //   unsigned int port;
  // } Data;

  Token(const size_t Line, const std::string &Str,
        const std::string::const_iterator It,
        std::string::const_iterator &ItNew);
  Token(const size_t Line, const TokenType::Type Type);
  const TokenType &getType(void) const;
  const std::string &getLexeme(void) const;
  size_t getLine(void) const;

private:
  const size_t _line;
  const TokenType &_type;
  std::string _lexeme;
  // Data _data;
};

std::ostream &operator<<(std::ostream &Os, const Token &Tkn);
