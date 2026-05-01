#include "Website.hpp"

#include "CompileTimeConstants.hpp"
#include "HttpMethods.hpp"
#include "Location.hpp"
#include <algorithm>
#include <cstddef>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <utility>

static void printLocations(std::ostream &Os, const Website &Site);
static void printInterfaces(std::ostream &Os, const Website &Site);
static void printRoot(std::ostream &Os, const Website &Site);
static void printAutoindex(std::ostream &Os, const Website &Site);
static void printAllow(std::ostream &Os, const Website &Site);
static void printErrorPages(std::ostream &Os, const Website &Site);
static void printMaxReqBody(std::ostream &Os, const Website &Site);
static int comparePath(const std::string &P1, const std::string &P2);

Website::Website(void)
    : _isRoot(false),
      _setMembers(0),
      _maxReqBody(MAX_REQUEST_BODY_DEFAULT),
      _root("/"),
      _autoindex(AUTOINDEX_DEFAULT),
      _allow(0),
      _type(None),
      _path("/"),
      _redirect(""),
      _returnCode(0),
      _returnPath("") {
}

Website::Website(const Website &Other)
    : _isRoot(Other._isRoot),
      _setMembers(Other._setMembers),
      _maxReqBody(Other._maxReqBody),
      _interfaces(Other._interfaces),
      _locations(Other._locations),
      _root(Other._root),
      _errorPages(Other._errorPages),
      _autoindex(Other._autoindex),
      _allow(Other._allow),
      _type(Other._type),
      _path(Other._path),
      _redirect(Other._redirect),
      _returnCode(Other._returnCode),
      _returnPath(Other._returnPath) {
}

Website &Website::operator=(const Website &Other) {
  if (this != &Other) {
    _isRoot = Other._isRoot;
    _setMembers = Other._setMembers;
    _maxReqBody = Other._maxReqBody;
    _interfaces = Other._interfaces;
    _locations = Other._locations;
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

Website::Website(const std::string &Path)
    : _isRoot(false),
      _setMembers(0),
      _maxReqBody(MAX_REQUEST_BODY_DEFAULT),
      _root("/"),
      _autoindex(AUTOINDEX_DEFAULT),
      _allow(0),
      _type(None),
      _path(Path),
      _redirect(""),
      _returnCode(0),
      _returnPath("") {
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

const std::string &Website::getRoot(void) const {
  return _root;
}

unsigned int Website::getMaxReqBody(void) const {
  return _maxReqBody;
}

bool Listen::operator==(const Listen &Other) const {
  return ip == Other.ip && port == Other.port;
}

const std::list<Listen> &Website::getInterfaces(void) const {
  return _interfaces;
}

std::ostream &operator<<(std::ostream &Os, const Listen &If) {
  If.print(Os);
  return Os;
}

void Listen::print(std::ostream &Os) const {
  Os << this->ip << ':' << this->port;
}

std::ostream &operator<<(std::ostream &Os, const Website &Site) {
  Site.print(Os);
  return Os;
}

void Website::print(std::ostream &Os) const {
  printInterfaces(Os, *this);
  printMaxReqBody(Os, *this);
  printRoot(Os, *this);
  printAutoindex(Os, *this);
  printAllow(Os, *this);
  printLocations(Os, *this);
  printErrorPages(Os, *this);
  switch (this->getType()) {
  case Website::Cgi:
    Os << "    cgi: " << this->getCgi() << '\n';
    break;
  case Website::Return:
    Os << "    return: " << this->getReturnCode() << " -> " << this->getReturnPath() << '\n';
    break;
  case Website::Redirect:
    Os << "    redirect: " << this->getRedirect() << '\n';
    break;
  case Website::None:
    break;
  }
}

unsigned int Website::getReturnCode(void) const {
  return _returnCode;
}

const std::string &Website::getReturnPath(void) const {
  return _returnPath;
}

static void printInterfaces(std::ostream &Os, const Website &Site) {
  if (!Site.isSetInterfaces())
    return;
  std::list<Listen>::const_iterator it = Site.getInterfaces().begin();
  while (it != Site.getInterfaces().end())
    Os << "listen: " << *it++ << '\n';
}

static void printLocations(std::ostream &Os, const Website &Site) {
  std::list<Website>::const_iterator it = Site.getLocations().begin();
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

void Website::setMaxReqBody(const unsigned int MaxBody) {
  _maxReqBody = MaxBody;
  _setMembers |= Website::MaxReqBody;
}

void Website::addAllow(const HttpMethod Method) {
  _allow |= Method;
  _setMembers |= Website::Allow;
}

bool Website::isSetAutoindex(void) const {
  return _setMembers & Website::Autoindex;
}

bool Website::isSetMaxReqBody(void) const {
  return _setMembers & Website::MaxReqBody;
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

void Website::addLocation(Website &Loc) {
  std::list<Website>::iterator it = _locations.begin();
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

void Website::addErrorPage(const unsigned int Code, const std::string &Path) {
  _errorPages.insert(std::pair<unsigned int, std::string>(Code, Path));
}

const char *Website::getErrorPage(const unsigned int Code) const {
  const std::map<unsigned int, std::string>::const_iterator it =
      _errorPages.find(Code);
  if (it == _errorPages.end())
    return NULL;
  return it->second.c_str();
}

const std::list<Website> &Website::getLocations(void) const {
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

static void printMaxReqBody(std::ostream &Os, const Website &Site) {
  Os << "max_request_body: " << Site.getMaxReqBody() << '\n';
}

PathInfo Website::getPathInfo(const std::string &Path) const {
  return PathInfo(*this, Path);
}

void Website::allowNone(void) {
  _setMembers |= Allow;
  _allow = 0;
}

void Website::allowAll(void) {
  addAllow(Head);
  addAllow(Get);
  addAllow(Post);
  addAllow(Delete);
}

Website::Type Website::getType(void) const {
  return _type;
}

void Website::setReturn(const unsigned int Code, const std::string &Url) {
  _returnCode = Code;
  _returnPath = Url;
  _type = Return;
}

void Website::setRedirect(const std::string &RedirectPath) {
  _redirect = RedirectPath;
  _type = Redirect;
}

void Website::setCgi(const std::string &CgiPath) {
  _redirect = CgiPath;
  _type = Cgi;
}

const std::string &Website::getPath(void) const {
  return _path;
}

const std::string &Website::getRedirect(void) const {
  return _redirect;
}

const std::string &Website::getCgi(void) const {
  return _redirect;
}

void Website::setAsRoot(void) {
  _isRoot = true;
}

bool Website::isRoot(void) const {
  return _isRoot;
}
