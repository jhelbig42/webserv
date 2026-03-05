#include "Website.hpp"

Website::Website(void) {
}

Website::Website(const Website &other) {
}

Website &Website::operator=(const Website &other) {
  if (this != &other) {
  }
  return *this;
}

Website::~Website(void) {
}

void addInterface(Listen &Interface) {
  _interfaces.push_back(Interface);
}

const std::list<Listen> &getInterfaces(void) const {
  return _interfaces;
}

std::ostream &operator<<(std::ostream &Os, const Listen &If) {
  Os << If.ip << ':' << If.port;
}

std::ostream &operator<<(std::ostream &Os, const Website &site) {
  std::list<Listen>::const_iterator it = _interfaces.begin();
  while (it != _interfaces.end()) {
    Os << *it << '\n';
  }
}
