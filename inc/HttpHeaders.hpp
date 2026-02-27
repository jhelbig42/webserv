#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>

class HttpHeaders {
public:
  HttpHeaders();
  HttpHeaders(const HttpHeaders &);
  HttpHeaders &operator=(const HttpHeaders &);
  ~HttpHeaders();

  typedef enum {
    ContentLength = (1u << 0),
    ContentType = (1u << 1),
	Date = (1u << 2)
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
    TextPlain
  } MediaType;

  bool isSet(const HeaderType Hdr) const;
  void unsetAll(void);

  void setContentLength(const off_t Length);
  off_t getContentLength(void) const;

  void setContentType(const char *extension);
  MediaType getContentType(void) const;


private:
  int _headersSet;
  off_t _contentLength;
  MediaType _contentType;
};

std::ostream &operator<<(std::ostream &Os, const HttpHeaders &Hdrs);
