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
    UnexpectedTokenException(const std::list<Token>::const_iterator It,
                             const std::string &Msg);
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
  bool isNextType(const TokenType::Type Type) const;
  const std::string &matchGetLexeme(TokenType::Type Type);

  Website expression(void);

  Website server(void);

  unsigned int parseUnsignedInt(void);
  void gap(void);
  void populateInterface(Listen &Interface);
  void parseListen(Website &Site);
  void parseRoot(Website &Site);
  void parseAutoindex(Website &Site);
  void parseAllow(Website &Site);

  void parseLocation(Website &Site);
  void parseRedirect(Website &Site);
  void parseCgi(Website &Site);
  void parseReturn(Website &Site);
  void parseMaxReqBody(Website &Site);

  void addIpv4(Listen &Interface);
  void addPort(Listen &Interface);

  void parseErrorPage(Website &Site);
  void validateEntry(const Website &Site);

  void parseEntry(Website &Website);
  std::string parseResource(void);
  std::string parseAbsPath(void);
  std::string parseWord(void);

  void throwTokenError(const std::string &Msg);

  bool parseOnOff(void);

  const Token &peek(void) const;
  void eat(void);

  Config &_config;
  Scanner _scan;
  unsigned int _line;

  std::list<Token>::const_iterator _it;
};
