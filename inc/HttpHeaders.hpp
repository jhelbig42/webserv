#pragma once

class HttpHeaders {
  public:
    HttpHeaders();
    HttpHeaders(const HttpHeaders&);
    HttpHeaders& operator=(const HttpHeaders&);
    ~HttpHeaders();

    typedef enum { ContentLength = (1u << 0) } HeaderType;

    bool isSet(const HeaderType Hdr);
    void setContentLength(const off_t Length);
    off_t getContentLength(void);

  private:
    unsigned long _headersSet;
    off_t _contentLength;
};

std::ostring &operator<<(std::ostring &Os, const HttpHeaders &Hdrs);
