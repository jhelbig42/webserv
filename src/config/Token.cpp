#include "Token.hpp"
#include "TokenType.hpp"
#include <cstddef>
#include <ostream>
#include <string>

const TokenType &Token::getType(void) const {
  return _type;
}

const std::string &Token::getLexeme(void) const {
  return _lexeme;
}

size_t Token::getLine(void) const {
  return _line;
}

size_t Token::getNum(void) const {
  return _num;
}

Token::Token(const size_t Line, const size_t Num, const std::string &Str,
             const std::string::const_iterator It,
             std::string::const_iterator &ItNew)
    : _line(Line), _num(Num), _type(TokenType::getTokenType(Str, It, ItNew)) {
  _lexeme = Str.substr(static_cast<std::string::size_type>(It - Str.begin()),
                       static_cast<std::string::size_type>(ItNew - It));
}

Token::Token(const size_t Line, const size_t Num, const TokenType::Type Type)
    : _line(Line), _num(Num), _type(TokenType::getTokenTypeId(Type)),
      _lexeme("") {
}

std::ostream &operator<<(std::ostream &Os, const Token &Tkn) {
  Os << "Line " << Tkn.getLine() << ": ";
  Os << Tkn.getType().identifier;
  Os << ": ";
  Os << Tkn.getLexeme() << '\n';
  return Os;
}
