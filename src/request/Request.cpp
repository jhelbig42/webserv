#include "Request.hpp"
#include <cstddef>
#include <string>

Request::Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid)
  : _method(Method), _resource(Resource), _majorVersion(MajorV), _minorVersion(MinorV), _valid(Valid) {
}

/// not implemented
void Request::parse(const char *buffer, const size_t bytes) {
  (void)buffer;
  (void)bytes;
}

bool Request::isValid() const {
	return _valid;
}

unsigned int Request::getMajorV() const {
	return _majorVersion;
}

unsigned int Request::getMinorV() const {
	return _minorVersion;
}

const std::string &Request::getResource() const {
	return _resource;
}

HttpMethod Request::getMethod() const {
	return _method;
}
