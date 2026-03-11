#include "Parser.hpp"

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

void Parser::parseListen(Website &site) {
  gap();
  Listen interface;
  populateInterface(interface);
  site.addInterface(interface);
}

void Parser::parseRoot(Website &site) {
  gap();
  const std::string root = parseAbsPath();
  site.setRoot(root);
}

void Parser::parseAutoindex(Website &site) {
  gap();
  site.setAutoindex(parseOnOff());
}

void Parser::addEntry(Website &Site) {
  if (match(TokenType::Listen)) {
    parseListen(Site);
  } else if (match(TokenType::Root)) {
    parseRoot(Site);
  } else if (match(TokenType::Autoindex)) {
    parseAutoindex(Site);
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
