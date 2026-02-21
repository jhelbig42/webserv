#pragma once

#include <iostream>
#include <sys/types.h>

class HttpHeaders {
public:
  HttpHeaders();
  HttpHeaders(const HttpHeaders &);
  HttpHeaders &operator=(const HttpHeaders &);
  ~HttpHeaders();

  typedef enum { ContentLength = (1u << 0) } HeaderType;

  bool isSet(const HeaderType Hdr) const;
  void setContentLength(const off_t Length);
  off_t getContentLength(void) const;

	void unsetAll(void);

private:
  unsigned long _headersSet;
  off_t _contentLength;
};

std::ostream &operator<<(std::ostream &Os, const HttpHeaders &Hdrs);
