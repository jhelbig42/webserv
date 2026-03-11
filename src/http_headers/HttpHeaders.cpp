#include "HttpHeaders.hpp"

#include <cstddef>
#include <cstring>
#include <string>

static const struct {
  std::string extension;
  HttpHeaders::MediaType type;
} globalMime[] = {{".css", HttpHeaders::TextCss},
                  {".csv", HttpHeaders::TextCsv},
                  {".gif", HttpHeaders::ImageGif},
                  {".htm", HttpHeaders::TextHtml},
                  {".html", HttpHeaders::TextHtml},
                  {".ico", HttpHeaders::ImageXIcon},
                  {".jpeg", HttpHeaders::ImageJpeg},
                  {".jpg", HttpHeaders::ImageJpeg},
                  {".js", HttpHeaders::ApplicationJavascript},
                  {".json", HttpHeaders::ApplicationJson},
                  {".png", HttpHeaders::ImagePng},
                  {".pdf", HttpHeaders::ApplicationPdf},
                  {".svg", HttpHeaders::ImageSvgXml},
                  {".txt", HttpHeaders::TextPlain}};

static const size_t globalMimeSize = sizeof(globalMime) / sizeof(*globalMime);

HttpHeaders::HttpHeaders(void) : _headersSet(0) {
}

HttpHeaders::HttpHeaders(const HttpHeaders &other)
    : _headersSet(other._headersSet), _contentLength(other._contentLength),
      _contentType(other._contentType) {
}

HttpHeaders &HttpHeaders::operator=(const HttpHeaders &other) {
  if (this != &other) {
    _headersSet = other._headersSet;
    _contentLength = other._contentLength;
    _contentType = other._contentType;
  }
  return *this;
}

HttpHeaders::~HttpHeaders(void) {
}

bool HttpHeaders::isSet(const HttpHeaders::HeaderType Hdr) const {
  return Hdr & _headersSet;
}

void HttpHeaders::unsetAll(void) {
  _headersSet = 0;
}

void HttpHeaders::setContentLength(const size_t Length) {
  _headersSet |= ContentLength;
  _contentLength = Length;
}

size_t HttpHeaders::getContentLength(void) const {
  return _contentLength;
}

void HttpHeaders::setContentType(const char *extension) {
  _headersSet |= ContentType;
  _contentType = Unknown;
  if (extension == NULL)
    return;
  for (size_t i = 0; i != globalMimeSize; ++i) {
    if (strcmp(extension, globalMime[i].extension.c_str()) == 0)
      _contentType = globalMime[i].type;
  }
  return;
}

HttpHeaders::MediaType HttpHeaders::getContentType(void) const {
  return _contentType;
}
