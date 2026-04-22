#include "Config.hpp"
#include "Parser.hpp"
#include "Website.hpp"
#include <iostream>
#include <list>

// class Config
Config::Config(const char *File) {
  Parser parser(*this, File);
  parser.parse();
}

void Config::addWebsite(const Website &Site) {
  _websites.push_back(Site);
}

std::ostream &operator<<(std::ostream &Os, const Config &Conf) {
  std::list<Website>::const_iterator it = Conf.getWebsites().begin();
  while (it != Conf.getWebsites().end()) {
    Os << *it++ << '\n';
  }
  return Os;
}

const std::list<Website> &Config::getWebsites(void) const {
  return _websites;
}
