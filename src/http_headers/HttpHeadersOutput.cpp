#include "HttpHeaders.hpp"

#include "HttpHeadersDefines.hpp"
#include <iostream>

static void writeMediaType(std::ostream &Os, const HttpHeaders::MediaType);

std::ostream &operator<<(std::ostream &Os, const HttpHeaders &Hdrs) {
  if (Hdrs.isSet(HttpHeaders::ContentLength))
    Os << NAME_CONTENT_LENGTH << ": " << Hdrs.getContentLength() << "\r\n";
  if (Hdrs.isSet(HttpHeaders::ContentType)) {
    Os << NAME_CONTENT_TYPE << ": ";
    writeMediaType(Os, Hdrs.getContentType());
    Os << "\r\n";
  }
  // other headers
  return Os;
}

static void writeMediaType(std::ostream &Os, const HttpHeaders::MediaType Type) {
  switch (Type) {
    case HttpHeaders::TextCss:
    Os << "text/css";
    break;
    case HttpHeaders::TextCsv:
    Os << "text/csv";
    break;
    case HttpHeaders::ImageGif:
    Os << "image/gif";
    break;
    case HttpHeaders::TextHtml:
    Os << "text/html";
    break;
    case HttpHeaders::ImageXIcon:
    Os << "image/x-icon";
    break;
    case HttpHeaders::ImageJpeg:
    Os << "image/jpeg";
    break;
    case HttpHeaders::ApplicationJavascript:
    Os << "application/javascript";
    break;
    case HttpHeaders::ApplicationJson:
    Os << "application/json";
    break;
    case HttpHeaders::ImagePng:
    Os << "image/png";
    break;
    case HttpHeaders::ApplicationPdf:
    Os << "application/pdf";
    break;
    case HttpHeaders::ImageSvgXml:
    Os << "image/svg+xml";
    break;
    case HttpHeaders::TextPlain:
    Os << "text/plain";
    break;
    case HttpHeaders::Unknown:
    Os << "application/octet-stream"; // see rfc1945 section 7.2.1
    break;
  }
}
