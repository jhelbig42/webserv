#include "Config.hpp"

#include "TokenType.hpp"
#include "Website.hpp"
#include <string>

Website Config::server(void) {
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

void Config::parseEntry(Website &Website) {
  skipSep();
  addEntry(Website);
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError();
  skipSep();
}

void Config::gap(void) {
  if (!sep())
    throwTokenError();
  skipSep();
}

void Config::populateInterface(Listen &Interface) {
  addIpv4(Interface);
  if (!match(TokenType::Colon))
    throwTokenError();
  addPort(Interface);
}

void Config::parseListen(Website &site) {
  gap();
  Listen interface;
  populateInterface(interface);
  site.addInterface(interface);
}

void Config::parseRoot(Website &site) {
  gap();
  const std::string root = parseAbsPath();
  site.setRoot(root);
}

void Config::parseAutoindex(Website &site) {
  gap();
  site.setAutoindex(parseOnOff());
}

void Config::addEntry(Website &Site) {
  if (match(TokenType::Listen)) {
    parseListen(Site);
  } else if (match(TokenType::Root)) {
    parseRoot(Site);
  } else if (match(TokenType::Autoindex)) {
    parseAutoindex(Site);
  } else
    throwTokenError();
}

bool Config::parseOnOff(void) {
  if (match(TokenType::On))
    return true;
  if (match(TokenType::Off))
    return false;
  throwTokenError();
  return false;
}

void Config::addIpv4(Listen &interface) {
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

void Config::addPort(Listen &interface) {
  try {
    interface.port = matchGetLexeme(TokenType::Number);
  } catch (...) {
    throwTokenError();
  }
}

std::string Config::parseAbsPath(void) {
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
