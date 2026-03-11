#include "Website.hpp"
#include <algorithm>
#include <list>
#include <ostream>

static void printInterfaces(std::ostream &Os, const Website &Site);
static void printRoot(std::ostream &Os, const Website &Site);
static void printAutoindex(std::ostream &Os, const Website &Site);

Website::Website(void): _setMembers(0), _autoindex(false) {
}

Website::Website(const Website &other)
    : _setMembers(other._setMembers), _interfaces(other._interfaces),
      _root(other._root), _autoindex(other._autoindex) {
}

Website &Website::operator=(const Website &other) {
  if (this != &other) {
    _setMembers = other._setMembers;
    _interfaces = other._interfaces;
    _root = other._root;
    _autoindex = other._autoindex;
  }
  return *this;
}

Website::~Website(void) {
}

void Website::addInterface(Listen &If) {
  if (std::find(_interfaces.begin(), _interfaces.end(), If) ==
      _interfaces.end())
    _interfaces.push_back(If);
  _setMembers |= Interfaces;
}

void Website::setRoot(const std::string &RootDir) {
  _root = RootDir;
  _setMembers |= Website::Root;
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

std::ostream &operator<<(std::ostream &Os, const Website &Site) {
  printInterfaces(Os, Site);
  printRoot(Os, Site);
  printAutoindex(Os, Site);
  return Os;
}

static void printInterfaces(std::ostream &Os, const Website &Site) {
  if (!Site.isSetInterfaces())
    return;
  std::list<Listen>::const_iterator it = Site.getInterfaces().begin();
  while (it != Site.getInterfaces().end())
    Os << "listen: " << *it++ << '\n';
}

static void printRoot(std::ostream &Os, const Website &Site) {
  if (!Site.isSetRoot())
    return;
  Os << "root: " << Site.getRoot() << '\n';
}

static void printAutoindex(std::ostream &Os, const Website &Site) {
  if (!Site.isSetAutoindex())
    return;
  Os << "autoindex: ";
  if (Site.getAutoindex())
    Os << "on";
  else
    Os << "off";
  Os << '\n';
}

bool Website::getAutoindex(void) const {
  return _autoindex;
}

void Website::setAutoindex(const bool IsOn) {
  _autoindex = IsOn;
  _setMembers |= Website::Autoindex;
}

bool Website::isSetAutoindex(void) const {
  return _setMembers & Website::Autoindex;
}

bool Website::isSetRoot(void) const {
  return _setMembers & Website::Root;
}

bool Website::isSetInterfaces(void) const {
  return _setMembers & Website::Interfaces;
}
