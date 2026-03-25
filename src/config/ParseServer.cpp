#include "Parser.hpp"

#include "HttpMethods.hpp"
#include "TokenType.hpp"
#include "Website.hpp"
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
  if (nextType() != TokenType::Number)
    throwTokenError();
  std::list<std::string> numbers;
  while (nextType() == TokenType::Number) {
    numbers.push_back(matchGetLexeme(TokenType::Number));
    skipSep();
  }
  Site.addErrorPage(numbers, parseResource);
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
    while (nextType() != TokenType::Newline &&
           nextType() != TokenType::Whitespace &&
           nextType() != TokenType::Semicolon &&
           nextType() != TokenType::Slash) {
      path += peek().getLexeme();
      eat();
    }
    if (sep() || nextType() == TokenType::Semicolon)
      return path;
  }
}

std::string Parser::parseAbsPath(void) {
  std::string path = matchGetLexeme(TokenType::Slash);
  if (sep() || nextType() == TokenType::Semicolon)
    return path;
  while (true) {
    if (nextType() == TokenType::Slash)
      throwTokenError();
    while (nextType() != TokenType::Newline &&
           nextType() != TokenType::Whitespace &&
           nextType() != TokenType::Semicolon &&
           nextType() != TokenType::Slash) {
      path += peek().getLexeme();
      eat();
    }
    if (!match(TokenType::Slash))
      throwTokenError();
    path += "/";
    if (sep() || nextType() == TokenType::Semicolon)
      return path;
  }
}
