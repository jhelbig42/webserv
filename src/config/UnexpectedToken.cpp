#include "Parser.hpp"

#include "Token.hpp"
#include <cstddef>
#include <list>
#include <sstream>
#include <string>

Parser::UnexpectedTokenException::UnexpectedTokenException(
    const std::list<Token>::const_iterator It, const std::string &Msg) {
  std::list<Token>::const_iterator it = It;
  // go to beginning of line
  while (it->getNum() > 1 && it->getLine() == It->getLine())
    --it;
  if (it->getLine() != It->getLine())
    ++it;
  // build line from tokens and get position of problematic token
  std::string lineStr("");
  size_t linePosition = 0;
  bool reachedPos = false;
  while (it->getType().type != TokenType::Eof &&
         it->getLine() == It->getLine()) {
    if (it != It && !reachedPos) {
      lineStr += it->getLexeme();
      linePosition += it->getLexeme().length();
    } else if (it == It) {
      lineStr += "\x1B[31m" + it->getLexeme() + "\033[0m";
      reachedPos = true;
    } else
      lineStr += it->getLexeme();
    ++it;
  }
  // construct indicator string
  std::string indicatorStr = lineStr.substr(0, linePosition);
  for (size_t i = 0; i != linePosition; ++i) {
    if (indicatorStr[i] != '\t')
      indicatorStr[i] = ' ';
  }
  std::ostringstream oss;
  oss << "Unexpected token in line " << It->getLine() << ":\n";
  if (It->getType().type != TokenType::Eof)
    oss << '\n';
  oss << lineStr << '\n';
  oss << "\x1B[31m" << indicatorStr;
  oss << std::string(It->getLexeme().length(), '^');
  oss << "\x1B[32m " << Msg << "\033[0m\n";
  _report = oss.str();
}

const char *Parser::UnexpectedTokenException::what() const throw() {
  return _report.c_str();
}

Parser::UnexpectedTokenException::~UnexpectedTokenException(void) throw() {
}
