#include <cctype>
#include <cstring>
#include <iostream>
#include <list>

struct TokenType {
  typedef enum {
    Eof,
    Semicolon,
    BracesLeft,
    BracesRight,
    Whitespace,
    Name
  } Type;
  Type type;
  bool (*isTypeChar)(const char);
};

class Token {
public:
  Token();
  Token(const std::string &Str, std::string::iterator &It);
  TokenType::Type getType(void) const;
  std::string getLexeme(void) const;

private:
  TokenType::Type _type;
  std::string _lexeme;
};

class Scanner {
public:
  explicit Scanner(std::string &source);
  ~Scanner();
  std::list<Token *> _tokens;

private:
  std::string &_source;
};

std::ostream &operator<<(std::ostream &Os, const Token &Tkn);
std::ostream &operator<<(std::ostream &Os, Scanner &Scan);
bool isEofChar(const char Ch);
bool isSemicolonChar(const char Ch);
bool isBracesLeftChar(const char Ch);
bool isBracesRightChar(const char Ch);
bool isWhitespaceChar(const char Ch);
bool isNameChar(const char Ch);
void advanceIt(const TokenType::Type Type, const std::string &Str, std::string::iterator &It);
static void printType(std::ostream &Os, const TokenType::Type Type);

Token::Token() {
}

TokenType::Type Token::getType(void) const {
  return _type;
}

std::string Token::getLexeme(void) const {
  return _lexeme;
}

static const TokenType globalTokenTypes[] =
    {{TokenType::Eof, &isEofChar},
     {TokenType::Semicolon, &isSemicolonChar},
     {TokenType::BracesLeft, &isBracesLeftChar},
     {TokenType::BracesRight, &isBracesRightChar},
     {TokenType::Whitespace, &isWhitespaceChar},
     {TokenType::Name, &isNameChar}};

static const size_t globalTokenTypesSize =
    sizeof(globalTokenTypes) / sizeof(*globalTokenTypes);

Scanner::~Scanner() {
  for (std::list<Token *>::iterator It = _tokens.begin(); It != _tokens.end();
       ++It)
    delete *It;
}

std::ostream &operator<<(std::ostream &Os, const Token &Tkn) {
  printType(Os, Tkn.getType());
  Os << ":\t";
  if (Tkn.getType() == TokenType::Name)
    Os << '\t';
  Os << Tkn.getLexeme() << '\n';
  return Os;
}

static void printType(std::ostream &Os, const TokenType::Type Type) {
  switch (Type) {
    case TokenType::Eof:
    Os << "Eof";
    break;
    case TokenType::Semicolon:
    Os << "Semicolon";
    break;
    case TokenType::BracesLeft:
    Os << "BracesLeft";
    break;
    case TokenType::BracesRight:
    Os << "BracesRight";
    break;
    case TokenType::Whitespace:
    Os << "Whitespace";
    break;
    case TokenType::Name:
    Os << "Name";
  }
}

Scanner::Scanner(std::string &Source) : _source(Source) {
  for (std::string::iterator It = _source.begin(); It != _source.end();) {
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


Token::Token(const std::string &Str, std::string::iterator &It) {
  size_t i = 0;
  while (i != globalTokenTypesSize) {
    if (globalTokenTypes[i].isTypeChar(*It)) {
      _type = globalTokenTypes[i].type;
      break;
    }
    ++i;
  }
  const std::string::iterator start = It;
  advanceIt(_type, Str, It);
  if (_type == TokenType::Name)
    _lexeme = Str.substr(start - Str.begin(), It - start);
}

void advanceIt(const TokenType::Type Type, const std::string &Str, std::string::iterator &It) {
  switch (Type) {
    case TokenType::Eof:
    return;
    case TokenType::Semicolon:
    case TokenType::BracesLeft:
    case TokenType::BracesRight:
    ++It;
    return;
    case TokenType::Whitespace:
    while (It != Str.end() && isWhitespaceChar(*It))
      ++It;
    return;
    case TokenType::Name:
    while (It != Str.end() && isNameChar(*It))
      ++It;
    return;
  }
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

int main(int argc, char **argv) {
  std::string s(argv[1], argv[1] + strlen(argv[1]));
  Scanner scan(s);
  std::cout << scan;
}
