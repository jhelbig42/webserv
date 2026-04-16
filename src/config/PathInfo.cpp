#include "Website.hpp"

#include "Location.hpp"
#include "Logging.hpp"

static bool isPrefix(const std::string &Prefix, const std::string &Str);
static bool isFullMatch(const std::string &Compare, const std::string &Str);
static bool match(const std::string &Path, const std::string &LocationPath);
static std::string substitutePath(const std::string &Path, const std::string &Substitute);

PathInfo::PathInfo(void)
    : _cgiPath(""), _realPath(""), _action(Default), _code(0), _allow(0) {
}

PathInfo::PathInfo(const PathInfo &Other)
    : _cgiPath(Other._cgiPath), _realPath(Other._realPath),
      _action(Other._action), _code(Other._code), _allow(Other._allow) {
}

PathInfo &PathInfo::operator=(const PathInfo &Other) {
  if (this != &Other) {
    _cgiPath = Other._cgiPath;
    _realPath = Other._realPath;
    _action = Other._action;
    _code = Other._code;
    _allow = Other._allow;
  }
  return *this;
}

PathInfo::~PathInfo(void) {
}

void PathInfo::populateFromLocation(const Location &Loc) {
  if (Loc.getType() == Location::Cgi) {
    _action = Cgi;
    _cgiPath = Loc.getCgi();
  } else if (Loc.getType() == Location::Return) {
    _code = Loc.getReturn().code;
    _realPath = Loc.getReturn().path;
  } else if (Loc.getType() == Location::Redirect) {
    _realPath = substitutePath(_realPath, Loc.getRedirect());
    resolveLocations(Loc.getLocations());
  }
  Logging::log2(Logging::Warning, __func__, ": Unreachable code reached!");
}

void PathInfo::resolveLocations(const std::list<Location> &Locations) {
  for (std::list<Location>::const_iterator it = Locations.begin();
       it != Locations.end(); ++it) {
    if (match(it->getPath(), Path)) {
      populateFromLocation(*it);
      break;
    }
  }
}

PathInfo::PathInfo(const Website &Site, const std::string &Path)
    : _cgiPath(""), _realPath(Path), _action(Default), _code(0),
      _allow(Site.getAllow()) {

  resolveLocations(Site.getLocations());

  if (_action != Return)
    _realPath = Site.getRealPath() + _realPath;
}

const std::string &PathInfo::getCgiPath(void) const {
  return _cgiPath;
}

const std::string &PathInfo::getRealPath(void) const {
  return _realPath;
}

const int PathInfo::getAllowed(void) const {
  return _allow;
}

Action PathInfo::getAction(void) const {
  return _action;
}

unsigned int PathInfo::getCode(void) const {
  return _code;
}

static bool isPrefix(const std::string &Prefix, const std::string &Str) {
  return Str.substr(0, Prefix.size()) == Prefix;
}

static bool isFullMatch(const std::string &Compare, const std::string &Str) {
  return Compare == Str;
}

static bool match(const std::string &Path, const std::string &LocationPath) {
  if (LocationPath[LocationPath.length() - 1] == '/')
    return isPrefix(LocationPath, Path);
  return isFullMatch(LocationPath, Path);
}

static std::string substitutePath(const std::string &Path, const std::string &Substitute) {
  if (Substitute[Substitute.length() - 1] == '/')
    return Substitute + Path.substr(Substitute.length());
  return Substitute;
}

