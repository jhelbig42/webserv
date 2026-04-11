#include "Parser.hpp"

#include "HttpMethods.hpp"
#include "TokenType.hpp"
#include "Website.hpp"
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <string>

Website Parser::server(void) {
  if (!match(TokenType::Server))
    throwTokenError();
  skipSep();
  if (!match(TokenType::BracesLeft))
    throwTokenError();
  Website newWebsite;
  while (!match(TokenType::BracesRight)) {
    parseEntry(newWebsite);
  }
  return newWebsite;
}

void Parser::parseEntry(Website &Website) {
  skipSep();
  addEntry(Website);
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError();
  skipSep();
}

void Parser::gap(void) {
  if (!sep())
    throwTokenError();
  skipSep();
}

void Parser::populateInterface(Listen &Interface) {
  addIpv4(Interface);
  if (!match(TokenType::Colon))
    throwTokenError();
  addPort(Interface);
}

void Parser::parseListen(Website &Site) {
  gap();
  Listen interface;
  populateInterface(interface);
  Site.addInterface(interface);
}

void Parser::parseRoot(Website &Site) {
  gap();
  const std::string root = parseAbsPath();
  Site.setRoot(root);
}

void Parser::parseAutoindex(Website &Site) {
  gap();
  Site.setAutoindex(parseOnOff());
}

void Parser::parseAllow(Website &Site) {
  gap();
  while (nextType() != TokenType::Semicolon) {
    if (match(TokenType::Asterisk)) {
      Site.addAllow(Head);
      Site.addAllow(Get);
      Site.addAllow(Post);
      Site.addAllow(Delete);
    }
    else if (match(TokenType::Head))
      Site.addAllow(Head);
    else if (match(TokenType::Get))
      Site.addAllow(Get);
    else if (match(TokenType::Post))
      Site.addAllow(Post);
    else if (match(TokenType::Delete))
      Site.addAllow(Delete);
    else
      throwTokenError();
    skipSep();
  }
}

void Parser::parseErrorPage(Website &Site) {
  gap();
  if (!isNextType(TokenType::Number))
    throwTokenError();
  std::list<std::string> numbers;
  while (nextType() == TokenType::Number) {
    numbers.push_back(matchGetLexeme(TokenType::Number));
    skipSep();
  }
  const std::string resource = parseResource();
  for (std::list<std::string>::const_iterator it = numbers.begin(); it != numbers.end(); ++it) {
    errno = 0;
    unsigned long num = strtoul(it->c_str(), NULL, 10);
    if (errno == ERANGE || num > std::numeric_limits<unsigned int>::max())
      continue;
    Site.addErrorPage(static_cast<unsigned int>(num), resource);
  }
}

void Parser::parseLocation(Website &Site) {
  gap();
  if (nextType() != TokenType::Slash)
    throwTokenError();
  std::string path = "";
  while (!isNextType(TokenType::Newline) &&
         !isNextType(TokenType::Whitespace) &&
         !isNextType(TokenType::BracesRight)) {
    path += peek().getLexeme();
    eat();
  }
  skipSep();
  Location newLocation(path);
  while (!match(TokenType::BracesRight)) {
    parseLocationEntry(newLocation);
  }
  Site.addLocation(newLocation);
}

void Parser::parseLocationEntry(Location &Loc) {
  skipSep();
  addLocationEntry(Loc);
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError();
  skipSep();
}

void Parser::addLocationEntry(Location &Loc) {
  if (match(TokenType::Return)) {
    parseReturn(Loc);
  } else if (match(TokenType::Redirect)) {
    parseRedirect(Loc);
  } else if (match(TokenType::Allow)) {
    parseLocationAllow(Loc);
  } else if (match(TokenType::Cgi)) {
    parseCgi(Loc);
  } else
    throwTokenError();
}

void Parser::parseReturn(Location &Loc) {
  ReturnData ret;
  if (!isNextType(TokenType::Number))
    throwTokenError();
  ret.code = peek().getLexeme();
  eat();
  skipSep();
  ret.url = parseWord();
  Loc.setReturn(ret);
}

void Parser::parseRedirect(Location &Loc) {
  std::string pathRedirect = parseWord();
  if (pathRedirect == "")
    throwTokenError();
  Loc.setRedirect(pathRedirect);
}

void Parser::parseCgi(Location &Loc) {
  std::string pathCgi = parseWord();
  if (pathCgi == "")
    throwTokenError();
  Loc.setCgi(pathCgi);
}

void Parser::parseLocationAllow(Location &Loc) {
  gap();
  while (!isNextType(TokenType::Semicolon)) {
    if (match(TokenType::Asterisk)) {
      Loc.addAllow(Head);
      Loc.addAllow(Get);
      Loc.addAllow(Post);
      Loc.addAllow(Delete);
    }
    else if (match(TokenType::Head))
      Loc.addAllow(Head);
    else if (match(TokenType::Get))
      Loc.addAllow(Get);
    else if (match(TokenType::Post))
      Loc.addAllow(Post);
    else if (match(TokenType::Delete))
      Loc.addAllow(Delete);
    else
      throwTokenError();
    skipSep();
  }
}

void Parser::addEntry(Website &Site) {
  if (match(TokenType::Listen)) {
    parseListen(Site);
  } else if (match(TokenType::Root)) {
    parseRoot(Site);
  } else if (match(TokenType::Autoindex)) {
    parseAutoindex(Site);
  } else if (match(TokenType::Allow)) {
    parseAllow(Site);
  } else if (match(TokenType::ErrorPage)) {
    parseErrorPage(Site);
  } else if (match(TokenType::Location)) {
    parseLocation(Site);
  } else
    throwTokenError();
}

bool Parser::parseOnOff(void) {
  if (match(TokenType::On))
    return true;
  if (match(TokenType::Off))
    return false;
  throwTokenError();
  return false;
}

void Parser::addIpv4(Listen &interface) {
  try {
    interface.ip = matchGetLexeme(TokenType::Number);
    interface.ip += matchGetLexeme(TokenType::Dot);
    interface.ip += matchGetLexeme(TokenType::Number);
    interface.ip += matchGetLexeme(TokenType::Dot);
    interface.ip += matchGetLexeme(TokenType::Number);
    interface.ip += matchGetLexeme(TokenType::Dot);
    interface.ip += matchGetLexeme(TokenType::Number);
  } catch (...) {
    throwTokenError();
  }
}

void Parser::addPort(Listen &interface) {
  try {
    interface.port = matchGetLexeme(TokenType::Number);
  } catch (...) {
    throwTokenError();
  }
}

std::string Parser::parseResource(void) {
  std::string path = matchGetLexeme(TokenType::Slash);
  if (sep() || nextType() == TokenType::Semicolon)
    return path;
  while (true) {
    if (nextType() == TokenType::Slash)
      throwTokenError();
    while (!isNextType(TokenType::Semicolon) &&
           !isNextType(TokenType::Newline) &&
           !isNextType(TokenType::Whitespace) &&
           !isNextType(TokenType::Slash)) {
      path += peek().getLexeme();
      eat();
    }
    if (sep() || nextType() == TokenType::Semicolon)
      return path;
  }
}

std::string Parser::parseAbsPath(void) {
  std::string path = matchGetLexeme(TokenType::Slash);
  if (sep() || isNextType(TokenType::Semicolon))
    return path;
  while (true) {
    if (nextType() == TokenType::Slash)
      throwTokenError();
    while (!isNextType(TokenType::Semicolon) &&
           !isNextType(TokenType::Newline) &&
           !isNextType(TokenType::Whitespace) &&
           !isNextType(TokenType::Slash)) {
      path += peek().getLexeme();
      eat();
    }
    if (!match(TokenType::Slash))
      throwTokenError();
    path += "/";
    if (sep() || isNextType(TokenType::Semicolon))
      return path;
  }
}
