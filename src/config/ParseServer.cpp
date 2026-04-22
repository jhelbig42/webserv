#include "Parser.hpp"

#include "HttpMethods.hpp"
#include "TokenType.hpp"
#include "Website.hpp"
#include <string>

Website Parser::server(void) {
  if (!match(TokenType::Server))
    throwTokenError("expected 'server'");
  skipSep();
  if (!match(TokenType::BracesLeft))
    throwTokenError("expected '{'");
  Website newWebsite;
  while (!match(TokenType::BracesRight)) {
    skipSep();
    if (isNextType(TokenType::Eof))
      throwTokenError("unexpected end of file");
    parseEntry(newWebsite);
    skipSep();
  }
  if (!newWebsite.isSetAllow())
    newWebsite.allowAll();
  return newWebsite;
}

void Parser::parseEntry(Website &Site) {
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
  } else if (match(TokenType::MaxRequestBody)) {
    parseMaxReqBody(Site);
  } else if (match(TokenType::Location)) {
    parseLocation(Site);
  } else
    throwTokenError("invalid Entry");
}

void Parser::gap(void) {
  if (!sep())
    throwTokenError("expected whitespace");
  skipSep();
}

void Parser::populateInterface(Listen &Interface) {
  addIpv4(Interface);
  if (!match(TokenType::Colon))
    throwTokenError("expected ':'");
  addPort(Interface);
}

void Parser::parseListen(Website &Site) {
  gap();
  Listen interface;
  populateInterface(interface);
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Site.addInterface(interface);
}

void Parser::parseRoot(Website &Site) {
  gap();
  const std::string root = parseAbsPath();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Site.setRoot(root);
}

void Parser::parseAutoindex(Website &Site) {
  gap();
  Site.setAutoindex(parseOnOff());
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
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
      throwTokenError("unrecognized HTTP method");
    skipSep();
  }
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
}

void Parser::parseErrorPage(Website &Site) {
  gap();
  if (!isNextType(TokenType::Number))
    throwTokenError("expected a number");
  std::list<unsigned int> numbers;
  while (nextType() == TokenType::Number) {
    numbers.push_back(parseUnsignedInt());
    skipSep();
  }
  const std::string resource = parseResource();
  for (std::list<unsigned int>::const_iterator it = numbers.begin();
       it != numbers.end(); ++it)
    Site.addErrorPage(*it, resource);
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
}

void Parser::parseLocation(Website &Site) {
  gap();
  if (nextType() != TokenType::Slash)
    throwTokenError("ecpected '/'");
  std::string path = "";
  while (!isNextType(TokenType::Newline) &&
         !isNextType(TokenType::Whitespace) &&
         !isNextType(TokenType::BracesLeft)) {
    path += peek().getLexeme();
    eat();
  }
  skipSep();
  Location newLocation(path);
  if (!match(TokenType::BracesLeft))
    throwTokenError("expected '{'");
  while (!match(TokenType::BracesRight)) {
    parseLocationEntry(newLocation);
  }
  Site.addLocation(newLocation);
}

void Parser::parseLocation(Location &Loc) {
  gap();
  if (nextType() != TokenType::Slash)
    throwTokenError("expected '/'");
  std::string path = "";
  while (!isNextType(TokenType::Newline) &&
         !isNextType(TokenType::Whitespace) &&
         !isNextType(TokenType::BracesLeft)) {
    path += peek().getLexeme();
    eat();
  }
  skipSep();
  Location newLocation(path);
  if (!match(TokenType::BracesLeft))
    throwTokenError("expected '{'");
  while (!match(TokenType::BracesRight)) {
    parseLocationEntry(newLocation);
  }
  Loc.addLocation(newLocation);
}

void Parser::validateLocationEnty(const Location &Loc) {
  if (Loc.getType() != Location::None &&
      (isNextType(TokenType::Return) || isNextType(TokenType::Cgi) || isNextType(TokenType::Redirect)))
    throwTokenError("location already has a type");
  if (Loc.getType() != Location::Redirect && isNextType(TokenType::Location))
    throwTokenError("can only nest locations that define redirects");
}

void Parser::parseLocationEntry(Location &Loc) {
  skipSep();
  validateLocationEnty(Loc);
  if (match(TokenType::Return)) {
    parseReturn(Loc);
  } else if (match(TokenType::Redirect)) {
    parseRedirect(Loc);
  } else if (match(TokenType::Allow)) {
    parseLocationAllow(Loc);
  } else if (match(TokenType::Cgi)) {
    parseCgi(Loc);
  } else if (match(TokenType::Location)) {
    parseLocation(Loc);
  } else
    throwTokenError("invalid location entry");
  skipSep();
}

void Parser::parseReturn(Location &Loc) {
  gap();
  ReturnData ret;
  unsigned int code = parseUnsignedInt();
  skipSep();
  const std::string url = parseWord();
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Loc.setReturn(code, url); 
}

void Parser::parseMaxReqBody(Website &Site) {
  gap();
  Site.setMaxReqBody(parseUnsignedInt());
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
}

void Parser::parseRedirect(Location &Loc) {
  gap();
  std::string pathRedirect = parseWord();
  if (pathRedirect == "")
    throwTokenError("not a valid redirect path");
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Loc.setRedirect(pathRedirect);
}

void Parser::parseCgi(Location &Loc) {
  gap();
  std::string pathCgi = parseResource();
  if (pathCgi == "")
    throwTokenError("not a valid cgi path");
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
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
      throwTokenError("unrecognized HTTP method");
    skipSep();
  }
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
}

bool Parser::parseOnOff(void) {
  if (match(TokenType::On))
    return true;
  if (match(TokenType::Off))
    return false;
  throwTokenError("expected 'on' or 'off'");
  return false;
}

void Parser::addIpv4(Listen &Interface) {
  try {
    Interface.ip = matchGetLexeme(TokenType::Number);
    Interface.ip += matchGetLexeme(TokenType::Dot);
    Interface.ip += matchGetLexeme(TokenType::Number);
    Interface.ip += matchGetLexeme(TokenType::Dot);
    Interface.ip += matchGetLexeme(TokenType::Number);
    Interface.ip += matchGetLexeme(TokenType::Dot);
    Interface.ip += matchGetLexeme(TokenType::Number);
  } catch (...) {
    throwTokenError("not a valid IP address");
  }
}

void Parser::addPort(Listen &Interface) {
  try {
    Interface.port = matchGetLexeme(TokenType::Number);
  } catch (...) {
    throwTokenError("expected a number");
  }
}

std::string Parser::parseResource(void) {
  std::string path = matchGetLexeme(TokenType::Slash);
  if (sep() || nextType() == TokenType::Semicolon)
    return path;
  while (true) {
    if (nextType() == TokenType::Slash)
      throwTokenError("expected a resource not a path");
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
      throwTokenError("expected '/'");
    while (!isNextType(TokenType::Semicolon) &&
           !isNextType(TokenType::Newline) &&
           !isNextType(TokenType::Whitespace) &&
           !isNextType(TokenType::Slash)) {
      path += peek().getLexeme();
      eat();
    }
    if (!match(TokenType::Slash))
      throwTokenError("expected '/'");
    path += "/";
    if (sep() || isNextType(TokenType::Semicolon))
      return path;
  }
}
