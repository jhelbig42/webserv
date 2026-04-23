#include "Website.hpp"

#include "CompileTimeConstants.hpp"
#include "HttpMethods.hpp"
#include "Location.hpp"
#include "Logging.hpp"
#include <list>
#include <ostream>
#include <string>

static void printAutoindex(std::ostream &Os, const PathInfo &Info);
static void printMaxReqBody(std::ostream &Os, const PathInfo &Info);
static void printRoot(std::ostream &Os, const PathInfo &Info);
static void printRoot(std::ostream &Os, const PathInfo &Info);
static void printErrorPages(std::ostream &Os, const Website &Info);
static bool isPrefix(const std::string &Prefix, const std::string &Str);
static bool isFullMatch(const std::string &Compare, const std::string &Str);
static bool match(const std::string &Path, const std::string &LocationPath);
static std::string substitutePath(const std::string &Path,
                                  const std::string &Substitute,
                                  const std::string &LocationPath);

PathInfo::PathInfo(const PathInfo &Other)
    : _cgiPath(Other._cgiPath), _realPath(Other._realPath),
      _action(Other._action), _code(Other._code), _allow(Other._allow),
      _maxReqBody(Other._maxReqBody), _root(Other._root),
      _errorPagesWebsite(Other._errorPagesWebsite),
      _errorPagesLocation(Other._errorPagesLocation),
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
    _errorPagesWebsite = Other._errorPagesWebsite;
    _errorPagesLocation = Other._errorPagesLocation;
    _autoindex = Other._autoindex;
  }
  return *this;
}

PathInfo::~PathInfo(void) {
}

void PathInfo::populateFromLocation(const Location &Loc) {
  if (Loc.isSetAllow())
    _allow = Loc.getAllow();
  if (Loc.isSetMaxReqBody())
    _maxReqBody = Loc.getMaxReqBody();
  if (Loc.isSetRoot())
    _root = Loc.getRoot();
  if (Loc.isSetAutoindex())
    _root = Loc.getAutoindex();
  _errorPagesLocation = &Loc.getErrorPages();
  if (Loc.getType() == Location::Cgi) {
    _action = Cgi;
    _cgiPath = Loc.getCgi();
  } else if (Loc.getType() == Location::Return) {
    _action = Return;
    _code = Loc.getReturn().code;
    _realPath = Loc.getReturn().url;
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

void PathInfo::resolveLocations(const std::list<Location> &Locations) {
  for (std::list<Location>::const_iterator it = Locations.begin();
       it != Locations.end(); ++it) {
    if (match(_realPath, it->getPath())) {
      populateFromLocation(*it);
      break;
    }
  }
}

PathInfo::PathInfo(const Website &Site, const std::string &Path)
    : _cgiPath(""), _realPath(Path), _action(Default), _code(0),
      _allow(Site.getAllow()), _maxReqBody(Site.getMaxReqBody()),
      _root(Site.getRoot()), _errorPagesWebsite(&Site.getErrorPages()),
      _errorPagesLocation(&Site.getErrorPages()),
      _autoindex(Site.getAutoindex()) {

  resolveLocations(Site.getLocations());

  if (_action != Return)
    _realPath =
        _root.substr(0, _root.length() - 1) + _realPath;
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
  if (Substitute[Substitute.length() - 1] == '/')
    return Substitute + Path.substr(LocationPath.length());
  return Substitute;
}

const char *PathInfo::getErrorPage(const unsigned int Code) const {
  std::map<unsigned int, std::string>::const_iterator it =
      _errorPagesLocation->find(Code);
  if (it != _errorPagesLocation->end())
    return it->second.c_str();
  it = _errorPagesWebsite->find(Code);
  if (it != _errorPagesWebsite->end())
    return it->second.c_str();
  return NULL;
}

static void printErrorPages(std::ostream &Os, const PathInfo &Info) {
  for (unsigned int i = 0; i != 1000; ++i) {
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

std::ostream &operator<<(std::ostream &Os, const PathInfo &Info) {
  Os << "class PathInfo: {\n";
  Os << "  error pages:\n";
  printErrorPages(Os, Info);
  printRoot(Os, Info);
  printMaxReqBody(Os, Info);
  printAutoindex(Os, Info);
  Os << "  allow:";
  if (Info.getAllowed() & Head)
    Os << " HEAD";
  if (Info.getAllowed() & Get)
    Os << " GET";
  if (Info.getAllowed() & Post)
    Os << " POST";
  if (Info.getAllowed() & Delete)
    Os << " DELETE";
  Os << '\n';
  switch (Info.getAction()) {
  case PathInfo::Return:
    Os << "  return: " << Info.getCode() << ": " << Info.getRealPath();
    break;
  case PathInfo::Cgi:
    Os << "  cgi: " << Info.getCgiPath() << ": " << Info.getRealPath();
    break;
  case PathInfo::Default:
    Os << "  default: " << Info.getRealPath();
    break;
  }
  return Os << "\n}\n";
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
