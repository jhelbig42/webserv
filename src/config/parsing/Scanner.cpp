#include "Scanner.hpp"

#include "Token.hpp"
#include "TokenType.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

Scanner::~Scanner() { }

Scanner::Scanner(const char *File): _numLines(1), _numTokens(0) {
  std::ifstream inf(File);
  if (!inf.is_open())
    throw std::runtime_error("Scanner: can not open file");
  std::string line;
  while (true) {
    std::getline(inf, line);
    if (!inf.good())
      break;
    scanLine(line);
    addNewline();
  }
  if (!inf.eof())
    throw std::runtime_error("Scanner: file I/O error");
  addEof();
}

void Scanner::addEof(void) {
  _tokens.push_back(Token(_numLines, ++_numTokens, TokenType::Eof));
}

void Scanner::addNewline(void) {
  _tokens.push_back(Token(_numLines++, ++_numTokens, TokenType::Newline));
}

void Scanner::scanLine(const std::string &Str) {
  std::string::const_iterator it = Str.begin();
  std::string::const_iterator itTmp;
  while (it != Str.end()) {
    _tokens.push_back(Token(_numLines, ++_numTokens, Str, it, itTmp));
    it = itTmp;
  }
}

std::ostream &operator<<(std::ostream &Os, const Scanner &Scan) {
  size_t line = 0;
  for (std::list<Token>::const_iterator it = Scan.getTokens().begin();
       it != Scan.getTokens().end(); ++it) {
    if ((*it).getLine() > line) {
      line = (*it).getLine();
      Os << "\n########### Line " << line << " ###########\n";
    }
    Os << *it;
  }
  return Os;
}

const std::list<Token> &Scanner::getTokens(void) const {
  return _tokens;
}

std::list<Token>::const_iterator Scanner::firstToken(void) const {
  return _tokens.begin();
}
