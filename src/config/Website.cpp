#include "Website.hpp"

Website::Website(void) {
}

Website::Website(const Website &other)
  : _interfaces(other._interfaces) {
}

Website &Website::operator=(const Website &other) {
  if (this != &other) {
    _interfaces = other._interfaces;
  }
  return *this;
}

Website::~Website(void) {
}

void Website::addInterface(Listen &Interface) {
  _interfaces.push_back(Interface);
}

const std::list<Listen> &Website::getInterfaces(void) const {
  return _interfaces;
}

std::ostream &operator<<(std::ostream &Os, const Listen &If) {
  return Os << If.ip << ':' << If.port;
}

std::ostream &operator<<(std::ostream &Os, const Website &site) {
  std::list<Listen>::const_iterator it = site.getInterfaces().begin();
  while (it != site.getInterfaces().end()) {
    Os << *it++ << '\n';
  }
  return Os;
}
