#include "Location.hpp"

#include "CompileTimeConstants.hpp"
#include "HttpMethods.hpp"
#include "Location.hpp"
#include "Website.hpp"
#include <algorithm>
#include <cstddef>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <utility>

static void printLocations(std::ostream &Os, const Location &Site);
static void printInterfaces(std::ostream &Os, const Location &Site);
static void printRoot(std::ostream &Os, const Location &Site);
static void printAutoindex(std::ostream &Os, const Location &Site);
static void printAllow(std::ostream &Os, const Location &Site);
static void printErrorPages(std::ostream &Os, const Location &Site);
static void printMaxReqBody(std::ostream &Os, const Location &Site);
static void printIndex(std::ostream &Os, const Location &Site);
static int comparePath(const std::string &P1, const std::string &P2);

Location::Location(void)
    : _isRoot(false), _setMembers(0), _maxReqBody(MAX_REQUEST_BODY_DEFAULT),
      _root("/"), _autoindex(AUTOINDEX_DEFAULT), _allow(0), _type(None),
      _path("/"), _redirect(""), _returnCode(0), _returnPath("") {
}

Location::Location(const Location &Other)
    : _isRoot(Other._isRoot), _setMembers(Other._setMembers),
      _maxReqBody(Other._maxReqBody), _interfaces(Other._interfaces),
      _locations(Other._locations), _index(Other._index), _root(Other._root),
      _errorPages(Other._errorPages), _autoindex(Other._autoindex),
      _allow(Other._allow), _type(Other._type), _path(Other._path),
      _redirect(Other._redirect), _returnCode(Other._returnCode),
      _returnPath(Other._returnPath) {
}

Location &Location::operator=(const Location &Other) {
  if (this != &Other) {
    _isRoot = Other._isRoot;
    _setMembers = Other._setMembers;
    _maxReqBody = Other._maxReqBody;
    _interfaces = Other._interfaces;
    _locations = Other._locations;
    _index = Other._index;
    _root = Other._root;
    _errorPages = Other._errorPages;
    _autoindex = Other._autoindex;
    _allow = Other._allow;
    _type = Other._type;
    _path = Other._path;
    _redirect = Other._redirect;
    _returnCode = Other._returnCode;
    _returnPath = Other._returnPath;
  }
  return *this;
}

Location::Location(const std::string &Path)
    : _isRoot(false), _setMembers(0), _maxReqBody(MAX_REQUEST_BODY_DEFAULT),
      _root("/"), _autoindex(AUTOINDEX_DEFAULT), _allow(0), _type(None),
      _path(Path), _redirect(""), _returnCode(0), _returnPath("") {
}

Location::~Location(void) {
}

void Location::addIndex(const std::string &Resource) {
  _index.push_back(Resource);
  _setMembers |= Index;
}

const std::list<std::string> &Location::getIndex(void) const {
  return _index;
}

void Location::addInterface(Listen &If) {
  if (std::find(_interfaces.begin(), _interfaces.end(), If) ==
      _interfaces.end())
    _interfaces.push_back(If);
  _setMembers |= Interfaces;
}

void Location::setRoot(const std::string &RootDir) {
  _root = RootDir;
  _setMembers |= Location::Root;
}

const std::string &Location::getRoot(void) const {
  return _root;
}

unsigned int Location::getMaxReqBody(void) const {
  return _maxReqBody;
}

bool Listen::operator==(const Listen &Other) const {
  return ip == Other.ip && port == Other.port;
}

const std::list<Listen> &Location::getInterfaces(void) const {
  return _interfaces;
}

std::ostream &operator<<(std::ostream &Os, const Listen &If) {
  If.print(Os);
  return Os;
}

void Listen::print(std::ostream &Os) const {
  Os << this->ip << ':' << this->port;
}

std::ostream &Location::print(std::ostream &Os) const {
  if (_isRoot)
    Os << "server {\n";
  else
    Os << "location " << _path << " {\n";
  printInterfaces(Os, *this);
  printMaxReqBody(Os, *this);
  printRoot(Os, *this);
  printAutoindex(Os, *this);
  printIndex(Os, *this);
  printAllow(Os, *this);
  printErrorPages(Os, *this);
  switch (this->getType()) {
  case Location::Cgi:
    Os << "    cgi: " << this->getCgi() << '\n';
    break;
  case Location::Return:
    Os << "    return: " << this->getReturnCode() << " -> "
       << this->getReturnPath() << '\n';
    break;
  case Location::Redirect:
    Os << "    redirect: " << this->getRedirect() << '\n';
    break;
  case Location::None:
    break;
  }
  printLocations(Os, *this);
  Os << "}\n";
  return Os;
}

unsigned int Location::getReturnCode(void) const {
  return _returnCode;
}

const std::string &Location::getReturnPath(void) const {
  return _returnPath;
}

bool Location::isSetIndex(void) const {
  return _setMembers & Index;
}

static void printIndex(std::ostream &Os, const Location &Site) {
  if (!Site.isSetIndex())
    return;
  std::list<std::string>::const_iterator it = Site.getIndex().begin();
  Os << "index:";
  while (it != Site.getIndex().end())
    Os << " " << *it++;
  Os << '\n';
}

static void printInterfaces(std::ostream &Os, const Location &Site) {
  if (!Site.isSetInterfaces())
    return;
  std::list<Listen>::const_iterator it = Site.getInterfaces().begin();
  while (it != Site.getInterfaces().end())
    Os << "listen: " << *it++ << '\n';
}

static void printLocations(std::ostream &Os, const Location &Site) {
  std::list<Location>::const_iterator it = Site.getLocations().begin();
  while (it != Site.getLocations().end()) {
    it->print(Os);
    ++it;
  }
}

static void printRoot(std::ostream &Os, const Location &Site) {
  if (!Site.isSetRoot())
    return;
  Os << "root: " << Site.getRoot() << '\n';
}

static void printAutoindex(std::ostream &Os, const Location &Site) {
  if (!Site.isSetAutoindex())
    return;
  Os << "autoindex: ";
  if (Site.getAutoindex())
    Os << "on";
  else
    Os << "off";
  Os << '\n';
}

static void printAllow(std::ostream &Os, const Location &Site) {
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

bool Location::getAutoindex(void) const {
  return _autoindex;
}

int Location::getAllow(void) const {
  return _allow;
}

void Location::setAutoindex(const bool IsOn) {
  _autoindex = IsOn;
  _setMembers |= Location::Autoindex;
}

void Location::setMaxReqBody(const unsigned int MaxBody) {
  _maxReqBody = MaxBody;
  _setMembers |= Location::MaxReqBody;
}

void Location::addAllow(const HttpMethod Method) {
  _allow |= Method;
  _setMembers |= Location::Allow;
}

bool Location::isSetAutoindex(void) const {
  return _setMembers & Location::Autoindex;
}

bool Location::isSetMaxReqBody(void) const {
  return _setMembers & Location::MaxReqBody;
}

bool Location::isSetRoot(void) const {
  return _setMembers & Location::Root;
}

bool Location::isSetInterfaces(void) const {
  return _setMembers & Location::Interfaces;
}

bool Location::isSetAllow(void) const {
  return _setMembers & Location::Allow;
}

static int comparePath(const std::string &P1, const std::string &P2) {
  if (P1.length() > P2.length())
    return -1;
  if (P1.length() < P2.length())
    return 1;
  if (P1 < P2)
    return -1;
  if (P1 > P2)
    return 1;
  return 0;
}

void Location::addLocation(Location &Loc) {
  std::list<Location>::iterator it = _locations.begin();
  while (it != _locations.end()) {
    if (Loc.getPath() == it->getPath()) {
      *it = Loc;
      return;
    }
    if (comparePath(Loc.getPath(), it->getPath()) == -1)
      break;
    ++it;
  }
  _locations.insert(it, Loc);
}

void Location::addErrorPage(const unsigned int Code, const std::string &Path) {
  _errorPages.insert(std::pair<unsigned int, std::string>(Code, Path));
}

const char *Location::getErrorPage(const unsigned int Code) const {
  const std::map<unsigned int, std::string>::const_iterator it =
      _errorPages.find(Code);
  if (it == _errorPages.end())
    return NULL;
  return it->second.c_str();
}

const std::list<Location> &Location::getLocations(void) const {
  return _locations;
}

static void printErrorPages(std::ostream &Os, const Location &Site) {
  for (std::map<unsigned int, std::string>::const_iterator it =
           Site.getErrorPages().begin();
       it != Site.getErrorPages().end(); ++it) {
    Os << it->first << " -> " << it->second << '\n';
  }
}

const std::map<unsigned int, std::string> &Location::getErrorPages(void) const {
  return _errorPages;
}

static void printMaxReqBody(std::ostream &Os, const Location &Site) {
  Os << "max_request_body: " << Site.getMaxReqBody() << '\n';
}

PathInfo Location::getPathInfo(const std::string &Path) const {
  return PathInfo(*this, Path);
}

void Location::allowNone(void) {
  _setMembers &= ~Allow;
  _allow = 0;
}

void Location::allowAll(void) {
  addAllow(Head);
  addAllow(Get);
  addAllow(Post);
  addAllow(Delete);
}

Location::Type Location::getType(void) const {
  return _type;
}

void Location::setReturn(const unsigned int Code, const std::string &Url) {
  _returnCode = Code;
  _returnPath = Url;
  _type = Return;
}

void Location::setRedirect(const std::string &RedirectPath) {
  _redirect = RedirectPath;
  _type = Redirect;
}

void Location::setCgi(const std::string &CgiPath) {
  _redirect = CgiPath;
  _type = Cgi;
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

void Location::setAsRoot(void) {
  _isRoot = true;
}

bool Location::isRoot(void) const {
  return _isRoot;
}
