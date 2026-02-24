#include <iostream>
#include <cctype>
#include <cstring>
#include <list>

typedef enum {
Eof = 0,
Semicolon = 1,
BracesLeft = 2,
BracesRight = 3,
Whitespace = 4,
Name = 5
} TokenType;

// static const struct { TokenType type; bool (*isTokenChar)(const char); } globalTokenChars = {
//   { Eof, &isEofChar },
//   { Semicolon, &isSemicolonChar },
//   { Braces_left, &isBraces_leftChar },
//   { Braces_right, &isBraces_rightChar },
//   { Whitespace, &isWhitespaceChar },
//   { Name, &isNameChar }
// };
//
//
class Token {
public:
  Token();
  Token(const std::string &Str, std::string::iterator &It);
  TokenType getType(void);
  std::string getLexeme(void);

private:
  static bool isType(const TokenType Type, const std::string &Str, std::string::iterator &It);
  TokenType _type;
  std::string _lexeme;
};

Token::Token() {}

TokenType Token::getType(void) {
  return _type;
}

std::string Token::getLexeme(void) {
  return _lexeme;
}

class Scanner {
public:
  explicit Scanner(std::string &source);
  ~Scanner();
  std::list<Token *> _tokens;
private:
  std::string &_source;
};

Scanner::~Scanner() {
  for (std::list<Token *>::iterator It = _tokens.begin(); It != _tokens.end(); ++It)
    delete *It;
}

std::ostream &operator<<(std::ostream &Os, Token &Tkn);
std::ostream &operator<<(std::ostream &Os, TokenType Type);
std::ostream &operator<<(std::ostream &Os, Scanner &Scan);
bool isEofChar(const char Ch);
bool isSemicolonChar(const char Ch);
bool isBraces_leftChar(const char Ch);
bool isBraces_rightChar(const char Ch);
bool isWhitespaceChar(const char Ch);
bool isNameChar(const char Ch);


std::ostream &operator<<(std::ostream &Os, Token &Tkn) {
  Os << Tkn.getType() << ":\t";
  if (Tkn.getType() == Name)
    Os << '\t';
  Os << Tkn.getLexeme() << '\n';
  return Os;
}

std::ostream &operator<<(std::ostream &Os, TokenType Type) {
  switch (Type) {
  case Eof:
    Os << "Eof";
    break;
  case Semicolon:
    Os << "Semicolon";
    break;
  case BracesLeft:
    Os << "BracesLeft";
    break;
  case BracesRight:
    Os << "BracesRight";
    break;
  case Whitespace:
    Os << "Whitespace";
    break;
  case Name:
    Os << "Name";
    break;
  }
  return Os;
}

Scanner::Scanner(std::string &Source)
  : _source(Source) {
  for (std::string::iterator It = _source.begin(); It != _source.end();) {
    Token *tkn = new Token(Source, It);
    _tokens.push_back(tkn);
  }
}

std::ostream &operator<<(std::ostream &Os, Scanner &Scan) {
  for (std::list<Token *>::iterator It = Scan._tokens.begin(); It != Scan._tokens.end(); ++It)
    Os << **It;
  return Os;
}

Token::Token(const std::string &Str, std::string::iterator &It) {
  for (int type = (int)Eof; type <= (int)Name; ++type) {
    if (isType((TokenType)type, Str, It)) {
      _type = (TokenType)type;
      break;
    }
  }
  if (_type == Eof)
    return;
  if (_type == Name) {
    const std::string::iterator start = It;
    while (It != Str.end() && isNameChar(*It))
      ++It;
    _lexeme = Str.substr(start - Str.begin(), It - start);
    return;
  }
  else if (_type == Whitespace) {
    while (It != Str.end() && isWhitespaceChar(*It))
      ++It;
    return;
  }
  ++It;
  return;
}

bool isEofChar(const char Ch) {
  return false;
}

bool isSemicolonChar(const char Ch) {
  return Ch == ';';
}

bool isBracesLeftChar(const char Ch) {
  return Ch == '{';
}

bool isBracesRightChar(const char Ch) {
  return Ch == '}';
}

bool isWhitespaceChar(const char Ch) {
  return isspace(Ch);
}

bool isNameChar(const char Ch) {
  static const char nameChar[] = "abcdefghijklmnopqrstuvwxyz";
  return strchr(nameChar, Ch);
}

bool Token::isType(const TokenType Type, const std::string &Str, std::string::iterator &It) {
  switch (Type) {
    case Eof: return isEofChar(*It);
    case Semicolon: return isSemicolonChar(*It);
    case BracesLeft: return isBracesLeftChar(*It);
    case BracesRight: return isBracesRightChar(*It);
    case Whitespace: return isWhitespaceChar(*It);
    case Name: return isNameChar(*It); 
  }
}

int main(int argc, char **argv) {
  std::string s(argv[1], argv[1] + strlen(argv[1]));
  Scanner scan(s);
  std::cout << scan;
}
