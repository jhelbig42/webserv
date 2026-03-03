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
  for (std::list<const Token *>::iterator it = _tokens.begin();
       it != _tokens.end(); ++it)
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
    addNewline(lineNumber);
  }
  if (inf.fail() && !inf.eof())
    throw std::runtime_error("Scanner: file I/O error");
  addEof(lineNumber);
}

void Scanner::addEof(const size_t Line) {
  const Token *const tkn = new Token(Line, TokenType::Eof);
  _tokens.push_back(tkn);
}

void Scanner::addNewline(const size_t Line) {
  const Token *const tkn = new Token(Line, TokenType::Newline);
  _tokens.push_back(tkn);
}

void Scanner::scanLine(const size_t Number, const std::string &Str) {
  std::string::const_iterator it = Str.begin();
  std::string::const_iterator itTmp;
  while (it != Str.end()) {
    const Token *const tkn = new Token(Number, Str, it, itTmp);
    _tokens.push_back(tkn);
    it = itTmp;
  }
}

std::ostream &operator<<(std::ostream &Os, const Scanner &Scan) {
  size_t line = 0;
  for (std::list<const Token *>::const_iterator it = Scan.getTokens().begin();
       it != Scan.getTokens().end(); ++it) {
    if ((**it).getLine() > line) {
      line = (**it).getLine();
      Os << "\n########### Line " << line << " ###########\n";
    }
    Os << **it;
  }
  return Os;
}

const std::list<const Token *> &Scanner::getTokens(void) const {
  return _tokens;
}
