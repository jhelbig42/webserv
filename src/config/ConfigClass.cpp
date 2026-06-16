#include "Config.hpp"
#include "Parser.hpp"
#include "Website.hpp"
#include <iostream>
#include <list>

// class Config
Config::Config(const char *File) {
  try {
    Parser parser(*this, File);
    parser.parse();
  } catch (const Parser::UnexpectedTokenException &e) {
    throw UnexpectedTokenException(e.what());
  } catch (...) {
    throw;
  }
}

void Config::addWebsite(const Website &Site) {
  _websites.push_back(Site);
}

const std::list<Website> &Config::getWebsites(void) const {
  return _websites;
}

Config::UnexpectedTokenException::UnexpectedTokenException(const std::string &Msg) {
  _report = Msg;
}

const char *Config::UnexpectedTokenException::what() const throw() {
  return _report.c_str();
}

Config::UnexpectedTokenException::~UnexpectedTokenException(void) throw() {
}
