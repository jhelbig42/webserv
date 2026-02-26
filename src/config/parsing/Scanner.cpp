#include "Scanner.hpp"

#include <cctype>
#include <cstring>
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
  for (std::list<Token *>::iterator It = _tokens.begin(); It != _tokens.end();
       ++It)
    delete *It;
}

std::ostream &operator<<(std::ostream &Os, Scanner &Scan) {
  size_t line = 0;
  for (std::list<Token *>::iterator It = Scan._tokens.begin();
       It != Scan._tokens.end(); ++It) {
    if ((**It).getLine() > line) {
      line = (**It).getLine();
      Os << "\n########### Line " << line << " ###########\n";
    }
    Os << **It;
  }
  return Os;
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
  Token *tkn = new Token(Line, TokenType::Eof);
  _tokens.push_back(tkn);
}

void Scanner::scanLine(const size_t Number, const std::string &Str) {
  std::string::const_iterator It = Str.begin();
  std::string::const_iterator ItTmp;
  while (It != Str.end()) {
    Token *tkn = new Token(Number, Str, It, ItTmp);
    _tokens.push_back(tkn);
    It = ItTmp;
  }
}

int main(int argc, char **argv) {
  Scanner scan(argv[1]);
  std::cout << scan;
}
