#include "Parser.hpp"

#include "HttpMethods.hpp"
#include "TokenType.hpp"
#include "Website.hpp"
#include <list>
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
    newWebsite.allowNone();
  newWebsite.setAsRoot();
  return newWebsite;
}

void Parser::parseEntry(Website &Site) {
  validateEntry(Site);
  if (match(TokenType::Return)) {
    parseReturn(Site);
  } else if (Site.isRoot() && match(TokenType::Listen)) {
    parseListen(Site);
  } else if (match(TokenType::Cgi)) {
    parseCgi(Site);
  } else if (match(TokenType::Redirect)) {
    parseRedirect(Site);
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

void Parser::parseMaxReqBody(Website &Site) {
  gap();
  Site.setMaxReqBody(parseUnsignedInt());
  skipSep();
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

void Parser::validateEntry(const Website &Site) {
  if (Site.getType() != Website::None &&
      (isNextType(TokenType::Return) || isNextType(TokenType::Cgi) ||
       isNextType(TokenType::Redirect)))
    throwTokenError("location already has a type");
  if (Site.getLocations().size() != 0) {
    if (isNextType(TokenType::Return))
      throwTokenError("locations that nest locations can not define return");
    if (isNextType(TokenType::Cgi))
      throwTokenError("locations that nest locations can not define cgi");
  }
  if (isNextType(TokenType::Location)) {
    if (Site.getType() == Website::Return)
      throwTokenError(
          "locations that define return can not nest other locations");
    if (Site.getType() == Website::Cgi)
      throwTokenError("locations that define cgi can not nest other locations");
  }
}

void Parser::parseLocation(Website &Site) { //NOLINT(misc-no-recursion)
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
  Website newLocation(path);
  if (!match(TokenType::BracesLeft))
    throwTokenError("expected '{'");
  while (!match(TokenType::BracesRight)) {
    parseEntry(newLocation);
  }
  Site.addLocation(newLocation);
}

void Parser::parseRedirect(Website &Site) {
  gap();
  const std::string pathRedirect = parseWord();
  if (pathRedirect == "")
    throwTokenError("not a valid redirect");
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Site.setRedirect(pathRedirect);
}

void Parser::parseCgi(Website &Site) {
  gap();
  const std::string pathCgi = parseResource();
  if (pathCgi == "")
    throwTokenError("not a valid cgi path");
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Site.setCgi(pathCgi);
}

void Parser::parseReturn(Website &Site) {
  gap();
  const unsigned int code = parseUnsignedInt();
  skipSep();
  const std::string url = parseWord();
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Site.setReturn(code, url); 
}
