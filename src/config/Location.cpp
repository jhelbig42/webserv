#include "Website.hpp"

Location::Location() : _setMembers(0), _path(""), _allow(0), _redirect("") {
}

Location::Location(const Location &Other)
    : _setMembers(Other._setMembers), _path(Other._path), _ret(Other._ret),
      _allow(Other._allow), _redirect(Other._redirect) {
}

Location &Location::operator=(const Location &Other) {
  if (this == &Other) {
    _setMembers = Other._setMembers;
    _ret = Other._ret;
    _allow = Other._allow;
    _redirect = Other._redirect;
  }
  return *this;
}

Location::~Location() {
}

explicit Location::Location(const std::string &Path)
    : _setMembers(0), _path(Path) {
}

void Location::setReturn(const std::string &Code, const std::string &Uri) {
  _redirect.code = Code;
  _redirect.uri = Uri;
  _setMembers |= Location::Redirect;
}

void Location::addAllow(const HttpMethod Method) {
  _allow |= Method;
  _setMembers |= Location::Allow;
}

void Location::setRedirect(const std::string &Redirect) {
  _redirect = Redirect;
  _setMembers |= Location::Redirect;
}

void Location::setCgi(const std::string &Cgi) {
  _cgi = Cgi;
  _setMembers |= Location::Cgi;
}

bool Location::getReturn(std::string &Code, std::string &Url) {
  if (!isSetRedirect())
    return false;
  Code = _redirect.code;
  Uri = _redirect.uri;
  return true;
}

bool Location::isAllowed(const HttpMethod Method) {
  return _allow & Method;
}

bool Location::getRedirect(std::string &Redirect) {
  if (!isSetRedirect())
    return false;
  Redirect = _redirect;
  return true;
}

bool Location::getCgi(std::string &Cgi) {
  if (!isSetCgi())
    return false;
  Cgi = _cgi;
  return true;
}

bool Location::isSetReturn(void) {
  return _setMembers & Location::Return;
}

bool Location::isSetAllow(void) {
  return _setMembers & Location::Allow;
}

bool Location::isSetRedirect(void) {
  return _setMembers & Location::Redirect;
}

bool Location::isSetCgi(void) {
  return _setMembers & Location::Cgi;
}
