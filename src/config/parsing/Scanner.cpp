#include "Scanner.hpp"

#include <cctype>
#include <cstring>
#include <string>
#include <iostream>
#include <list>

const TokenType &Token::getType(void) const {
  return _type;
}

const std::string &Token::getLexeme(void) const {
  return _lexeme;
}

Scanner::~Scanner() {
  for (std::list<Token *>::iterator It = _tokens.begin(); It != _tokens.end();
       ++It)
    delete *It;
}

std::ostream &operator<<(std::ostream &Os, const Token &Tkn) {
  Os << Tkn.getType().identifier;
  Os << ":\t";
  if (Tkn.getType().type == TokenType::Name)
    Os << '\t';
  Os << Tkn.getLexeme() << '\n';
  return Os;
}

Scanner::Scanner(const std::string &Source) : _source(Source) {
  std::string::const_iterator It = _source.begin();
  while (It != _source.end()) {
    Token *tkn = new Token(Source, It);
    _tokens.push_back(tkn);
  }
}

std::ostream &operator<<(std::ostream &Os, Scanner &Scan) {
  for (std::list<Token *>::iterator It = Scan._tokens.begin();
       It != Scan._tokens.end(); ++It)
    Os << **It;
  return Os;
}

Token::Token(const std::string &Str, std::string::const_iterator &It)
    : _type(TokenType::getTokenType(Str, It)) {
  const std::string::const_iterator start = It;
  It = _type.advanceIt(Str, It);
  _lexeme = Str.substr(start - Str.begin(), It - start);
}

int main(int argc, char **argv) {
  std::string s(argv[1], argv[1] + strlen(argv[1]));
  Scanner scan(s);
  std::cout << scan;
}
