#include "Parser.hpp"

#include "Token.hpp"
#include <cstddef>
#include <list>
#include <sstream>
#include <string>

Parser::UnexpectedTokenException::UnexpectedTokenException(
    const std::list<Token>::const_iterator It, const std::string &Msg) {
  std::list<Token>::const_iterator it = It;
  while (it->getNum() > 1 && it->getLine() == It->getLine())
    --it;
  if (it->getLine() != It->getLine())
    ++it;
  std::string lineStr("");
  size_t linePosition = 0;
  bool reachedPos = false;
  do {
    if (it != It && !reachedPos) {
      lineStr += it->getLexeme();
      linePosition += it->getLexeme().length();
    } else if (it == It) {
      lineStr += "\x1B[31m" + it->getLexeme() + "\033[0m";
      reachedPos = true;
    } else
      lineStr += it->getLexeme();
    ++it;
  } while (it->getType().type != TokenType::Eof &&
           it->getLine() == It->getLine());
  std::string showStr = lineStr.substr(0, linePosition + 1);
  for (size_t i = 0; i != linePosition; ++i) {
    if (showStr[i] != '\t')
      showStr[i] = ' ';
  }
  showStr[linePosition] = '^';
  std::ostringstream oss;
  oss << "Unexpected token in line " << It->getLine() << ":\n";
  oss << lineStr << '\n';
  oss << "\x1B[31m" << showStr << std::string(It->getLexeme().length() - 1, '^');
  oss << "\x1B[32m " << Msg << "\033[0m";
  _report = oss.str();
}

const char *Parser::UnexpectedTokenException::what() const throw() {
  return _report.c_str();
}

Parser::UnexpectedTokenException::~UnexpectedTokenException(void) throw() {
}
