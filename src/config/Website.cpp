#include "Website.hpp"
#include "Location.hpp"
#include <string>

Website::Website(const Location &Loc): _website(Loc) {
}

Website::~Website(void) {
}

PathInfo Website::getPathInfo(const std::string &Path) const {
  return _website.getPathInfo(Path);
}

const std::list<Listen> &Website::getInterfaces(void) const {
  return _website.getInterfaces();
}

std::ostream &Website::print(std::ostream &Os) const {
  return Os << this->_website.print(Os);
}

std::ostream &operator<<(std::ostream &Os, const Website &Site) {
  return Site.print(Os);
}
