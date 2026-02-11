#include "Request.hpp"

Request::Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid)
  : _method(Method), _resource(Resource), _majorVersion(MajorV), _minorVersion(MinorV), _valid(Valid) {
}

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
