#include "Location.hpp"
#include <ostream>

static void printLocations(std::ostream &Os, const Location &Loc);

Location::Location() : _type(None), _path(""), _allowSet(false), _allow(0) {
}

Location::Location(const Location &Other)
    : _type(Other._type), _path(Other._path), _return(Other._return),
      _allowSet(Other._allowSet), _allow(Other._allow),
      _redirect(Other._redirect), _locations(Other._locations) {
}

Location &Location::operator=(const Location &Other) {
  if (this != &Other) {
    _type = Other._type;
    _path = Other._path;
    _return = Other._return;
    _allowSet = Other._allowSet;
    _allow = Other._allow;
    _redirect = Other._redirect;
    _locations = Other._locations;
  }
  return *this;
}

Location::~Location() {
}

Location::Location(const std::string &Path)
    : _type(None), _path(Path), _allowSet(false), _allow(0) {
}

Location::Type Location::getType(void) const {
  return _type;
}

void Location::setReturn(const unsigned int Code, const std::string &Url) {
  _return.code = Code;
  _return.url = Url;
  _type = Return;
}

void Location::addAllow(const HttpMethod Method) {
  _allow |= Method;
  _allowSet = true;
}

bool Location::isSetAllowed(void) const {
  return _allowSet;
}

void Location::setRedirect(const std::string &RedirectPath) {
  _redirect = RedirectPath;
  _type = Redirect;
}

void Location::setCgi(const std::string &CgiPath) {
  _redirect = CgiPath;
  _type = Cgi;
}

const ReturnData &Location::getReturn(void) const {
  return _return;
}

const std::string &Location::getPath(void) const {
  return _path;
}

const std::string &Location::getRedirect(void) const {
  return _redirect;
}

const std::string &Location::getCgi(void) const {
  return _redirect;
}

bool Location::isAllowed(const HttpMethod Method) const {
  return _allow & Method;
}

std::ostream &operator<<(std::ostream &Os, const Location &Loc) {
  Os << "location " << Loc.getPath() << " {\n";
  if (Loc.isSetAllowed()) {
    Os << "  allow:";
    if (Loc.isAllowed(Post))
      Os << " POST";
    if (Loc.isAllowed(Get))
      Os << " GET";
    if (Loc.isAllowed(Delete))
      Os << " DELETE";
    if (Loc.isAllowed(Head))
      Os << " HEAD";
    Os << '\n';
  }
  switch (Loc.getType()) {
    case Location::Cgi:
      Os << "    cgi: " << Loc.getCgi() << '\n';
      break;
    case Location::Return:
      Os << "    return: " << Loc.getReturn() << '\n';
      break;
    case Location::Redirect:
      Os << "    redirect: " << Loc.getRedirect() << '\n';
      break;
    case Location::None:
      break;
  }
  printLocations(Os, Loc);
  Os << "}";
  return Os;
}

std::ostream &operator<<(std::ostream &Os, const ReturnData &Ret) {
  Os << Ret.code << ' ' << Ret.url;
  return Os;
}

const std::list<Location> &Location::getLocations(void) const {
  return _locations;
}

static void printLocations(std::ostream &Os, const Location &Loc) {
  std::list<Location>::const_iterator it = Loc.getLocations().begin();
  while (it != Loc.getLocations().end()) {
    Os << *it << '\n';
    ++it;
  }
}

void Location::addLocation(Location &Loc) {
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
}
