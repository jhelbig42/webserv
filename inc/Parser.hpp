#pragma once

#include "Config.hpp"

class Parser {
public:
  Parser();
  Parser(const Parser &);
  Parser &operator=(const Parser &);
  ~Parser();

  Parser(Config &Conf, const char *File);
  void parse(void);

  class UnexpectedTokenException : public std::exception {
  public:
    explicit UnexpectedTokenException(const std::list<Token>::const_iterator It);
    ~UnexpectedTokenException(void) throw();
    const char *what(void) const throw();
  private:
    std::string _report;
  };

private:
  bool sep(void);
  void skipSep(void);
  bool match(const TokenType::Type Type);
  bool noMatch(const TokenType::Type Type);
  TokenType::Type nextType(void) const;
  const std::string &matchGetLexeme(TokenType::Type Type);

  Website expression(void);

  Website server(void);

  void gap(void);
  void populateInterface(Listen &Interface);
  void parseListen(Website &Site);
  void parseRoot(Website &Site);
  void parseAutoindex(Website &Site);
  void parseAllow(Website &Site);

  void addEntry(Website &Site);

  void addIpv4(Listen &Interface);
  void addPort(Listen &Interface);

  void parseEntry(Website &Website);
  std::string parseAbsPath(void);

  void throwTokenError(void);

  bool parseOnOff(void);

  const Token &peek(void) const;
  void eat(void);

  Config &_config;
  Scanner _scan;
  unsigned int _line;

  std::list<Token>::const_iterator _it;
};
