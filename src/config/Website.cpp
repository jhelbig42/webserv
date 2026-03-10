#include "Website.hpp"
#include <algorithm>
#include <list>

Website::Website(void) {
}

Website::Website(const Website &other)
  : _interfaces(other._interfaces), _root(other._root) {
}

Website &Website::operator=(const Website &other) {
  if (this != &other) {
    _interfaces = other._interfaces;
    _root = other._root;
  }
  return *this;
}

Website::~Website(void) {
}

void Website::addInterface(Listen &If) {
  if (std::find(_interfaces.begin(), _interfaces.end(), If) == _interfaces.end())
    _interfaces.push_back(If);
}

void Website::setRoot(const std::string &Root) {
  _root = Root;
}

std::string Website::getRoot(void) const {
  return _root;
}

bool Listen::operator==(const Listen &other) const {
  return ip == other.ip && port == other.port;
}

const std::list<Listen> &Website::getInterfaces(void) const {
  return _interfaces;
}

std::ostream &operator<<(std::ostream &Os, const Listen &If) {
  return Os << If.ip << ':' << If.port;
}

std::ostream &operator<<(std::ostream &Os, const Website &site) {
  Os << "root: " << site.getRoot() << '\n';
  std::list<Listen>::const_iterator it = site.getInterfaces().begin();
  while (it != site.getInterfaces().end()) {
    Os << *it++ << '\n';
  }
  return Os;
}

bool Website::getAutoindex(void) const {
  return _autoindex;
}

void Website::setAutoindex(const bool IsOn) {
  _autoindex = IsOn;
}
