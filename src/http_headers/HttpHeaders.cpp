#include "HttpHeaders.hpp"
#include "HttpHeadersDefines.hpp"
#include <iostream>
#include <sys/types.h>

HttpHeaders::HttpHeaders(void) : _headersSet(0) {
}

HttpHeaders::HttpHeaders(const HttpHeaders &other)
    : _headersSet(other._headersSet), _contentLength(other._contentLength) {
}

HttpHeaders &HttpHeaders::operator=(const HttpHeaders &other) {
  if (this != &other) {
    _headersSet = other._headersSet;
    _contentLength = other._contentLength;
  }
  return *this;
}

HttpHeaders::~HttpHeaders(void) {
}

bool HttpHeaders::isSet(const HttpHeaders::HeaderType Hdr) const {
  return Hdr & _headersSet;
}

void HttpHeaders::setContentLength(const off_t Length) {
  _headersSet |= ContentLength;
  _contentLength = Length;
}

off_t HttpHeaders::getContentLength(void) const {
  return _contentLength;
}

std::ostream &operator<<(std::ostream &Os, const HttpHeaders &Hdrs) {
  if (Hdrs.isSet(HttpHeaders::ContentLength))
    Os << NAME_CONTENT_LENGTH << ": " << Hdrs.getContentLength() << "\r\n";
  // other headers
  return Os;
}
