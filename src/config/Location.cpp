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

explicit Location::Location(const std::string &Path): _path(Path) {
}

void Location::setReturn(const std::string &RootDir) {
}

void Location::addAllow(const HttpMethod Method) {
}

void Location::setRedirect(const std::string &Redirect) {
}

void Location::setCgi(const std::string &Cgi) {
}

void Location::getReturn(int &Code, std::string &Url) {
}

bool Location::isAllowed(const HttpMethod Method) {
}

void Location::getRedirect(std::string &Redirect) {
}

void Location::getCgi(std::string &Cgi) {
}

bool Location::isSetReturn(void) {
}

bool Location::isAddAllow(void) {
}

bool Location::isSetRedirect(void) {
}

bool Location::isSetCgi(void) {
}
