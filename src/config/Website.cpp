#include "Website.hpp"
#include <algorithm>
#include <list>
#include <ostream>

static void printLocations(std::ostream &Os, const Website &Site);
static void printInterfaces(std::ostream &Os, const Website &Site);
static void printRoot(std::ostream &Os, const Website &Site);
static void printAutoindex(std::ostream &Os, const Website &Site);
static void printAllow(std::ostream &Os, const Website &Site);
static void printErrorPages(std::ostream &Os, const Website &Site);

Website::Website(void)
    : _setMembers(0), _root(""), _autoindex(false), _allow(0) {
}

Website::Website(const Website &Other)
    : _setMembers(Other._setMembers), _interfaces(Other._interfaces),
      _locations(Other._locations), _root(Other._root),
      _errorPages(Other._errorPages), _autoindex(Other._autoindex),
      _allow(Other._allow) {
}

Website &Website::operator=(const Website &Other) {
  if (this != &Other) {
    _setMembers = Other._setMembers;
    _interfaces = Other._interfaces;
    _locations = Other._locations;
    _root = Other._root;
    _errorPages = Other._errorPages;
    _autoindex = Other._autoindex;
    _allow = Other._allow;
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

bool Listen::operator==(const Listen &Other) const {
  return ip == Other.ip && port == Other.port;
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
  printAllow(Os, Site);
  printLocations(Os, Site);
  printErrorPages(Os, Site);
  return Os;
}

static void printInterfaces(std::ostream &Os, const Website &Site) {
  if (!Site.isSetInterfaces())
    return;
  std::list<Listen>::const_iterator it = Site.getInterfaces().begin();
  while (it != Site.getInterfaces().end())
    Os << "listen: " << *it++ << '\n';
}

static void printLocations(std::ostream &Os, const Website &Site) {
  std::list<Location>::const_iterator it = Site.getLocations().begin();
  while (it != Site.getLocations().end()) {
    Os << *it << '\n';
    ++it;
  }
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

static void printAllow(std::ostream &Os, const Website &Site) {
  if (!Site.isSetAllow())
    return;
  Os << "allow:";
  if (Site.getAllow() & Head)
    Os << " HEAD";
  if (Site.getAllow() & Get)
    Os << " GET";
  if (Site.getAllow() & Post)
    Os << " POST";
  if (Site.getAllow() & Delete)
    Os << " DELETE";
  Os << '\n';
}

bool Website::getAutoindex(void) const {
  return _autoindex;
}

int Website::getAllow(void) const {
  return _allow;
}

void Website::setAutoindex(const bool IsOn) {
  _autoindex = IsOn;
  _setMembers |= Website::Autoindex;
}

void Website::addAllow(const HttpMethod Method) {
  _allow |= Method;
  _setMembers |= Website::Allow;
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

bool Website::isSetAllow(void) const {
  return _setMembers & Website::Allow;
}

void Website::addLocation(Location &Loc) {
  std::list<Location>::iterator it = _locations.begin();
  while (it != _locations.end()) {
    if (Loc.getPath() == it->getPath()) {
      *it = Loc;
      return;
    }
    if (Loc.getPath() < it->getPath())
      break;
    ++it;
  }
  _locations.insert(it, Loc);
  _setMembers |= Website::Locations;
}

void Website::addErrorPage(const unsigned int Code, const std::string &Path) {
  _errorPages.insert(std::pair<unsigned int, std::string>(Code, Path));
}

const char *Website::getErrorPage(const unsigned int Code) {
  const std::map<unsigned int, std::string>::const_iterator it =
      _errorPages.find(Code);
  if (it == _errorPages.end())
    return NULL;
  return it->second.c_str();
}

const std::list<Location> &Website::getLocations(void) const {
  return _locations;
}

static void printErrorPages(std::ostream &Os, const Website &Site) {
  for (std::map<unsigned int, std::string>::const_iterator it =
           Site.getErrorPages().begin();
       it != Site.getErrorPages().end(); ++it) {
    Os << it->first << " -> " << it->second << '\n';
  }
}

const std::map<unsigned int, std::string> &Website::getErrorPages(void) const {
  return _errorPages;
}
