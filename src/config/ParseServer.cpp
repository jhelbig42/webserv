#include "Parser.hpp"

#include "HttpMethods.hpp"
#include "Location.hpp"
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
    parseEntry(newLocation);
  }
  Site.addLocation(newLocation);
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
