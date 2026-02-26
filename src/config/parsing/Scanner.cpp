#include "Scanner.hpp"

#include "Token.hpp"
#include "TokenType.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <list>
#include <stdexcept>
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

Scanner::~Scanner() {
  for (std::list<Token *>::iterator it = _tokens.begin(); it != _tokens.end();
       ++it)
    delete *it;
}

Scanner::Scanner(const char *File) {
  std::ifstream inf(File);
  if (!inf.is_open())
    throw std::runtime_error("Scanner: can not open file");
  std::string line;
  size_t lineNumber = 0;

  while (true) {
    std::getline(inf, line);
    if (!inf.good())
      break;
    scanLine(++lineNumber, line);
  }
  if (inf.fail() && !inf.eof())
    throw std::runtime_error("Scanner: file I/O error");
  addEof(lineNumber);
}

void Scanner::addEof(const size_t Line) {
  Token *const tkn = new Token(Line, TokenType::Eof);
  _tokens.push_back(tkn);
}

void Scanner::scanLine(const size_t Number, const std::string &Str) {
  std::string::const_iterator it = Str.begin();
  std::string::const_iterator itTmp;
  while (it != Str.end()) {
    Token *tkn = new Token(Number, Str, it, itTmp);
    _tokens.push_back(tkn);
    it = itTmp;
  }
}

std::ostream &operator<<(std::ostream &Os, const Scanner &Scan) {
  size_t line = 0;
  for (std::list<Token *>::const_iterator it = Scan._tokens.begin();
       it != Scan._tokens.end(); ++it) {
    if ((**it).getLine() > line) {
      line = (**it).getLine();
      Os << "\n########### Line " << line << " ###########\n";
    }
    Os << **it;
  }
  return Os;
}
