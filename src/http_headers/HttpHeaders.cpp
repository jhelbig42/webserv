#include "HttpHeaders.hpp"

HttpHeaders::HttpHeaders(void)
  : _headersSet(0) {
}

HttpHeaders::HttpHeaders(const HttpHeaders& other)
  : _headersSet(other._headersSet), _contentLength(other._contentLength) {
}

HttpHeaders& HttpHeaders::operator=(const HttpHeaders& other) {
  if (this != &other) {
    _headersSet = other._headersSet;
    _contentLength = other._contentLength;
  }
  return *this;
}

HttpHeaders::~HttpHeaders(void) {
}

bool HttpHeaders::isSet(const HttpHeaders::HeaderType Hdr) {
  return Hdr & _headersSet;
}

void HttpHeaders::setContentLength(const off_t Length) {
  _headersSet |= ContentLength;
  _contentLength = Length;
}

off_t HttpHeaders::getContentLength(void) {
  return _contentLength;
}

std::ostring &HttpHeaders::operator<<(std::ostring &Os, const HttpHeaders &Hdrs) {
  if (Hdrs.isSet(HttpHeaders::ContentLength)
    Os << NAME_CONTENT_LENGTH << ": " << Hdrs.getContentLength() << "\r\n";
  // other headers
}
