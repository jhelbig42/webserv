#include "Location.hpp"

#include "CompileTimeConstants.hpp"
#include "HttpMethods.hpp"
#include <list>
#include <ostream>

static void printLocations(std::ostream &Os, const Location &Loc);

Location::Location()
    : _type(None), _setMembers(0), _path(""), _return(ReturnData()), _allow(0),
      _redirect(""), _maxReqBody(MAX_REQUEST_BODY_DEFAULT), _root("/"),
      _autoindex(AUTOINDEX_DEFAULT) {
}

Location::Location(const Location &Other)
    : _type(Other._type), _setMembers(Other._setMembers), _path(Other._path),
      _return(Other._return), _allow(Other._allow), _redirect(Other._redirect),
      _locations(Other._locations), _maxReqBody(Other._maxReqBody),
      _root(Other._root), _errorPages(Other._errorPages),
      _autoindex(Other._autoindex) {
}
Location &Location::operator=(const Location &Other) {
  if (this != &Other) {
    _type = Other._type;
    _path = Other._path;
    _return = Other._return;
    _allow = Other._allow;
    _redirect = Other._redirect;
    _locations = Other._locations;
    _maxReqBody = Other._maxReqBody;
    _errorPages = Other._errorPages;
    _root = Other._root;
    _autoindex = Other._autoindex;
  }
  return *this;
}

Location::~Location() {
}

Location::Location(const std::string &Path)
    : _type(None), _setMembers(0), _path(Path), _return(ReturnData()),
      _allow(0), _redirect(""), _maxReqBody(MAX_REQUEST_BODY_DEFAULT),
      _root("/"), _autoindex(AUTOINDEX_DEFAULT) {
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
  _setMembers |= Allow;
}

bool Location::isSetAllow(void) const {
  return _setMembers & Allow;
}

bool Location::isSetRoot(void) const {
  return _setMembers & Root;
}

bool Location::isSetAutoindex(void) const {
  return _setMembers & Autoindex;
}

bool Location::isSetMaxReqBody(void) const {
  return _setMembers & MaxReqBody;
}

void Location::setRoot(const std::string &RootDir) {
  _root = RootDir;
  _setMembers |= Root;
}

void Location::setAutoindex(const bool isOn) {
  _autoindex = isOn;
  _setMembers |= Autoindex;
}

void Location::setMaxReqBody(const unsigned int MaxBody) {
  _maxReqBody = MaxBody;
  _setMembers |= MaxReqBody;
}

void Location::addErrorPage(const unsigned int Code, const std::string &Path) {
  _errorPages.insert(std::pair<unsigned int, std::string>(Code, Path));
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

const std::string &Location::getRoot(void) const {
  return _root;
}

unsigned int Location::getMaxReqBody(void) const {
  return _maxReqBody;
}

bool Location::getAutoindex(void) const {
  return _autoindex;
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
  if (Loc.isSetAllow()) {
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

int Location::getAllow(void) const {
  return _allow;
}

const std::map<unsigned int, std::string> &Location::getErrorPages(void) const {
  return _errorPages;
}

const char *Location::getErrorPage(const unsigned int Code) const {
  const std::map<unsigned int, std::string>::const_iterator it =
      _errorPages.find(Code);
  if (it == _errorPages.end())
    return NULL;
  return it->second.c_str();
}
