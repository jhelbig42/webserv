#include "Config.hpp"

Website Config::server(void) {
  if (!match(TokenType::Server))
    throwTokenError();
  skipSep();
  if (!match(TokenType::BracesLeft))
    throwTokenError();
  Website newWebsite;
  while (!match(TokenType::BracesRight)) {
    skipSep();
    addEntry(newWebsite);
    skipSep();
    if (!match(TokenType::Semicolon))
      throwTokenError();
    skipSep();
  }
  return newWebsite;
}

void Config::addEntry(Website &site) {
  if (match(TokenType::Listen)) {
    if (!sep())
      throwTokenError();
    skipSep();
    Listen interface;
    addIpv4(interface);
    if (!match(TokenType::Colon))
      throwTokenError();
    addPort(interface);
    site.addInterface(interface);
  } else if (match(TokenType::Root)) {
    if (!sep())
      throwTokenError();
    skipSep();
    const std::string root = parseAbsPath();
    site.setRoot(root);
  } else if (match(TokenType::Autoindex)) {
    if (!sep())
      throwTokenError();
    skipSep();
    site.setAutoindex(parseOnOff());
  }
  else
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
  if (sep() || peek().getType().type == TokenType::Semicolon)
    return path;
  while (true) {
    if (peek().getType().type == TokenType::Slash)
      throwTokenError();
    while (peek().getType().type != TokenType::Newline
           && peek().getType().type != TokenType::Whitespace
           && peek().getType().type != TokenType::Semicolon
           && peek().getType().type != TokenType::Slash) {
      path += peek().getLexeme();
      eat();
    }
    if (!match(TokenType::Slash))
      throwTokenError();
    path += "/";
    if (sep() || peek().getType().type == TokenType::Semicolon)
      return path;
  }
}

