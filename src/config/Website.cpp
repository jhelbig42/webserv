#include "Website.hpp"
#include "Location.hpp"
#include <string>

Website::Website(void): _website(NULL) {
}

Website::Website(const Website &Other): _website(new Location(*Other._website)) {
}

Website &Website::operator=(const Website &Other) {
  if (this != &Other) {
    if (_website != NULL) {
      delete _website;
      _website = NULL;
    }
    if (Other._website != NULL) 
      _website = new Location(*Other._website);
  }
  return *this;
}

Website::Website(const Location &Loc) {
  _website = new Location(Loc);
}

Website::~Website(void) {
  delete _website;
}

PathInfo Website::getPathInfo(const std::string &Path) const {
  return _website->getPathInfo(Path);
}

const std::list<Listen> &Website::getInterfaces(void) const {
  return _website->getInterfaces();
}

std::ostream &Website::print(std::ostream &Os) const {
  return Os << this->_website->print(Os);
}

std::ostream &operator<<(std::ostream &Os, const Website &Site) {
  return Site.print(Os);
}
