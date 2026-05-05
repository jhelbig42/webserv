#include "Website.hpp"

#include "HttpMethods.hpp"
#include "Location.hpp"
#include "Logging.hpp"
#include <cstddef>
#include <list>
#include <map>
#include <ostream>
#include <string>

#define MAX_ERROR_CODE_PRINT 1000

static void printIndex(std::ostream &Os, const PathInfo &Info);
static void printAutoindex(std::ostream &Os, const PathInfo &Info);
static void printMaxReqBody(std::ostream &Os, const PathInfo &Info);
static void printRoot(std::ostream &Os, const PathInfo &Info);
static void printRoot(std::ostream &Os, const PathInfo &Info);
static void printErrorPages(std::ostream &Os, const Location &Info);
static bool isPrefix(const std::string &Prefix, const std::string &Str);
static bool isFullMatch(const std::string &Compare, const std::string &Str);
static bool match(const std::string &Path, const std::string &LocationPath);
static std::string substitutePath(const std::string &Path,
                                  const std::string &Substitute,
                                  const std::string &LocationPath);

PathInfo::PathInfo(void)
    : _cgiPath(""), _realPath(""),
      _action(Default), _code(0), _allow(0),
      _maxReqBody(0), _root(""),
      _autoindex(false) {
}

PathInfo::PathInfo(const PathInfo &Other)
    : _cgiPath(Other._cgiPath), _realPath(Other._realPath),
      _action(Other._action), _code(Other._code), _allow(Other._allow),
      _maxReqBody(Other._maxReqBody), _root(Other._root),
      _errorPages(Other._errorPages), _index(Other._index),
      _autoindex(Other._autoindex) {
}

PathInfo &PathInfo::operator=(const PathInfo &Other) {
  if (this != &Other) {
    _cgiPath = Other._cgiPath;
    _realPath = Other._realPath;
    _action = Other._action;
    _code = Other._code;
    _allow = Other._allow;
    _maxReqBody = Other._maxReqBody;
    _root = Other._root;
    _errorPages = Other._errorPages;
    _index = Other._index;
    _autoindex = Other._autoindex;
  }
  return *this;
}

PathInfo::~PathInfo(void) {
}

PathInfo::PathInfo(const Location &Site, const std::string &Path)
    : _cgiPath(""), _realPath(Path), _action(Default), _code(0),
      _allow(Site.getAllow()), _maxReqBody(Site.getMaxReqBody()),
      _root(Site.getRoot()), _autoindex(Site.getAutoindex()) {

  _errorPages.push_front(&Site.getErrorPages());

  resolveLocations(Site.getLocations());

  if (_action != Return)
    _realPath = _root.substr(0, _root.length() - 1) + _realPath;
  
  if (_realPath[_realPath.length() - 1] != '/')
    return;
  for (std::list<std::string>::iterator it = _index.begin();
       it != _index.end();
       ++it) {
    if ((*it)[0] == '/')
      *it = Site.getRoot() + it->substr(1);
    else
      *it = _realPath + *it;
  }
}

void PathInfo::populateFromLocation(
    const Location &Loc) { // NOLINT(misc-no-recursion)
  if (Loc.isSetAllow())
    _allow = Loc.getAllow();
  if (Loc.isSetMaxReqBody())
    _maxReqBody = Loc.getMaxReqBody();
  if (Loc.isSetRoot())
    _root = Loc.getRoot();
  if (Loc.isSetAutoindex())
    _autoindex = Loc.getAutoindex();
  _errorPages.push_front(&Loc.getErrorPages());
  if (Loc.isSetIndex())
    _index = Loc.getIndex();
  if (Loc.getType() == Location::Cgi) {
    _action = Cgi;
    _cgiPath = Loc.getCgi();
  } else if (Loc.getType() == Location::Return) {
    _action = Return;
    _code = Loc.getReturnCode();
    _realPath = Loc.getReturnPath();
  } else if (Loc.getType() == Location::Redirect) {
    _action = Default;
    _realPath = substitutePath(_realPath, Loc.getRedirect(), Loc.getPath());
    resolveLocations(Loc.getLocations());
  } else if (Loc.getType() == Location::None) {
    resolveLocations(Loc.getLocations());
  } else {
    logging::log2(logging::Warning, __func__, ": Unreachable code reached!");
  }
}

void PathInfo::resolveLocations(
    const std::list<Location> &Locations) { // NOLINT(misc-no-recursion)
  for (std::list<Location>::const_iterator it = Locations.begin();
       it != Locations.end(); ++it) {
    if (match(_realPath, it->getPath())) {
      populateFromLocation(*it);
      break;
    }
  }
}

const std::string &PathInfo::getCgiPath(void) const {
  return _cgiPath;
}

const std::string &PathInfo::getRealPath(void) const {
  return _realPath;
}

unsigned int PathInfo::getMaxReqBody(void) const {
  return _maxReqBody;
}

const std::string &PathInfo::getRoot(void) const {
  return _root;
}

bool PathInfo::getAutoindex(void) const {
  return _autoindex;
}

int PathInfo::getAllowed(void) const {
  return _allow;
}

PathInfo::Action PathInfo::getAction(void) const {
  return _action;
}

unsigned int PathInfo::getCode(void) const {
  return _code;
}

static bool isPrefix(const std::string &Prefix, const std::string &Str) {
  return Str.substr(0, Prefix.size()) == Prefix;
}

static bool match(const std::string &Path, const std::string &LocationPath) {
  if (LocationPath[LocationPath.length() - 1] == '/')
    return isPrefix(LocationPath, Path);
  return isFullMatch(LocationPath, Path);
}

static std::string substitutePath(const std::string &Path,
                                  const std::string &Substitute,
                                  const std::string &LocationPath) {
  if (LocationPath[LocationPath.length() - 1] == '/')
    return Substitute + Path.substr(LocationPath.length());
  return Substitute;
}

const char *PathInfo::getErrorPage(const unsigned int Code) const {
  std::list<const std::map<unsigned int, std::string> *>::const_iterator itLst =
      _errorPages.begin();
  std::map<unsigned int, std::string>::const_iterator itMap;
  while (itLst != _errorPages.end()) {
    itMap = (*itLst)->find(Code);
    if (itMap != (*itLst)->end())
      return itMap->second.c_str();
    ++itLst;
  }
  return NULL;
}

static void printErrorPages(std::ostream &Os, const PathInfo &Info) {
  for (unsigned int i = 0; i != MAX_ERROR_CODE_PRINT; ++i) {
    const char *page = Info.getErrorPage(i);
    if (page == NULL)
      continue;
    Os << "  " << i << " -> " << page << '\n';
  }
}

static void printAutoindex(std::ostream &Os, const PathInfo &Info) {
  Os << "  autoindex: ";
  if (Info.getAutoindex())
    Os << "on";
  else
    Os << "off";
  Os << '\n';
}

static void printMaxReqBody(std::ostream &Os, const PathInfo &Info) {
  Os << "  max_request_body: " << Info.getMaxReqBody() << '\n';
}

static void printRoot(std::ostream &Os, const PathInfo &Info) {
  Os << "  root: " << Info.getRoot() << '\n';
}

static void printIndex(std::ostream &Os, const PathInfo &Info) {
  Os << "  index:";
  for (std::list<std::string>::const_iterator it = Info.getIndex().begin();
       it != Info.getIndex().end(); ++it)
    Os << ' ' << *it;
}

const std::list<std::string> &PathInfo::getIndex(void) const {
  return _index;
}

std::ostream &operator<<(std::ostream &Os, const PathInfo &Info) {
  Info.print(Os);
  return Os;
}

void PathInfo::print(std::ostream &Os) const {
  Os << "class PathInfo: {\n";
  Os << "  error pages:\n";
  printErrorPages(Os, *this);
  printRoot(Os, *this);
  printMaxReqBody(Os, *this);
  printAutoindex(Os, *this);
  printIndex(Os, *this);
  Os << "  allow:";
  if (this->getAllowed() & Head)
    Os << " HEAD";
  if (this->getAllowed() & Get)
    Os << " GET";
  if (this->getAllowed() & Post)
    Os << " POST";
  if (this->getAllowed() & Delete)
    Os << " DELETE";
  Os << '\n';
  switch (this->getAction()) {
  case PathInfo::Return:
    Os << "  return: " << this->getCode() << ": " << this->getRealPath();
    break;
  case PathInfo::Cgi:
    Os << "  cgi: " << this->getCgiPath() << ": " << this->getRealPath();
    break;
  case PathInfo::Default:
    Os << "  default: " << this->getRealPath();
    break;
  }
  Os << "\n}\n";
}

static bool isFullMatch(const std::string &Pat, const std::string &Str) {

  std::string::const_iterator checkpoint = Pat.end();
  std::string::const_iterator itPat = Pat.begin();
  std::string::const_iterator itStr = Str.begin();

  while (true) {
    while (*itPat == '*') {
      checkpoint = itPat;
      ++itPat;
    }
    if (itStr == Str.end())
      return itPat == Pat.end();
    if (*itPat == *itStr)
      ++itPat;
    else if (checkpoint != Pat.end() && *(checkpoint + 1) == *itStr)
      itPat = checkpoint + 2;
    else if (checkpoint != Pat.end())
      itPat = checkpoint;
    else
      return false;
    ++itStr;
  }
}
