#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>

class HttpHeaders {
public:
  HttpHeaders();
  HttpHeaders(const HttpHeaders &Other);
  HttpHeaders &operator=(const HttpHeaders &Other);
  ~HttpHeaders();

  typedef enum {
    ContentLength = (1u << 0),
    ContentType = (1u << 1)
  } HeaderType;

  typedef enum {
    Unknown,
    TextCss,
    TextCsv,
    ImageGif,
    TextHtml,
    ImageXIcon,
    ImageJpeg,
    ApplicationJavascript,
    ApplicationJson,
    ImagePng,
    ApplicationPdf,
    ImageSvgXml,
    TextPlain,
    ApplicationSh,
    TextPython
  } MediaType;

  bool isSet(const HeaderType Hdr) const;
  void unsetAll(void);

  void setContentLength(const size_t Length);
  size_t getContentLength(void) const;

  void setContentType(const char *Extension);
  MediaType getContentType(void) const;


private:
  int _headersSet;
  size_t _contentLength;
  MediaType _contentType;
};

std::ostream &operator<<(std::ostream &Os, const HttpHeaders &Hdrs);
