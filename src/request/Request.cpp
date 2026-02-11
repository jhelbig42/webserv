#include "Request.hpp"

/// not implemented
void Request::parse(const char *buffer, const size_t bytes) {
  (void)buffer;
  (void)bytes;
}

const bool Request::isValid() const {
	return _valid;
}

const unsigned int Request::getVersionMajor() const {
	return _versionMajor;
}

const unsigned int Request::getVersionMinor() const {
	return _versionMinor;
}

const std::string &Request::getResource() const {
	return _resource;
}

const Request::getMethod() const {
	return _method;
}
