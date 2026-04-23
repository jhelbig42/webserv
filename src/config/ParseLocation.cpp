#include "Parser.hpp"
#include "Location.hpp"

void Parser::parseEntry(Location &Loc) {
  skipSep();
  validateLocationEnty(Loc);
  if (match(TokenType::Return)) {
    parseReturn(Loc);
  } else if (match(TokenType::Root)) {
    parseRoot(Loc);
  } else if (match(TokenType::Autoindex)) {
    parseAutoindex(Loc);
  } else if (match(TokenType::Redirect)) {
    parseRedirect(Loc);
  } else if (match(TokenType::ErrorPage)) {
    parseErrorPage(Loc);
  } else if (match(TokenType::MaxRequestBody)) {
    parseMaxReqBody(Loc);
  } else if (match(TokenType::Allow)) {
    parseAllow(Loc);
  } else if (match(TokenType::Cgi)) {
    parseCgi(Loc);
  } else if (match(TokenType::Location)) {
    parseLocation(Loc);
  } else
    throwTokenError("invalid location entry");
  skipSep();
}

void Parser::validateLocationEnty(const Location &Loc) {
  if (Loc.getType() != Location::None &&
      (isNextType(TokenType::Return) || isNextType(TokenType::Cgi) ||
       isNextType(TokenType::Redirect)))
    throwTokenError("location already has a type");
  if (Loc.getLocations().size() != 0) {
    if (isNextType(TokenType::Return))
      throwTokenError("locations that nest locations can not define return");
    if (isNextType(TokenType::Cgi))
      throwTokenError("locations that nest locations can not define cgi");
  }
  if (isNextType(TokenType::Location)) {
    if (Loc.getType() == Location::Return)
      throwTokenError(
          "locations that define return can not nest other locations");
    if (Loc.getType() == Location::Cgi)
      throwTokenError("locations that define cgi can not nest other locations");
  }
}

void Parser::parseMaxReqBody(Location &Loc) {
  gap();
  Loc.setMaxReqBody(parseUnsignedInt());
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
}

void Parser::parseRoot(Location &Loc) {
  gap();
  const std::string root = parseAbsPath();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Loc.setRoot(root);
}

void Parser::parseAutoindex(Location &Loc) {
  gap();
  Loc.setAutoindex(parseOnOff());
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
}

void Parser::parseErrorPage(Location &Loc) {
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
    Loc.addErrorPage(*it, resource);
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
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
    parseEntry(newLocation);
  }
  Loc.addLocation(newLocation);
}

void Parser::parseRedirect(Location &Loc) {
  gap();
  std::string pathRedirect = parseWord();
  if (pathRedirect == "")
    throwTokenError("not a valid redirect");
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Loc.setRedirect(pathRedirect);
}

void Parser::parseCgi(Location &Loc) {
  gap();
  const std::string pathCgi = parseResource();
  if (pathCgi == "")
    throwTokenError("not a valid cgi path");
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Loc.setCgi(pathCgi);
}

void Parser::parseAllow(Location &Loc) {
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

void Parser::parseReturn(Location &Loc) {
  gap();
  const unsigned int code = parseUnsignedInt();
  skipSep();
  const std::string url = parseWord();
  skipSep();
  if (!match(TokenType::Semicolon))
    throwTokenError("expected ';'");
  Loc.setReturn(code, url); 
}
