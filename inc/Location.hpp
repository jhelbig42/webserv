#pragma once

#include "HttpMethods.hpp"

struct Return {
  std::string code;
  std::string url;
};

class Location {
public:
  typedef enum {
    None,
    Cgi,
    Return,
    Redirect,
  } Type;

  Location();
  Location(const Location &Other);
  Location &operator=(const Location &Other);
  ~Location();

  explicit Location(const std::string &Path);

  Type getType(void) const;
  void setReturn(const std::string &RootDir);
  void addAllow(const HttpMethod Method);
  void setRedirect(const std::string &Redirect);
  void setCgi(const std::string &Cgi);

  bool isAllowed(const HttpMethod Method) const;
  const Return &getReturn(void) const;
  const std::string &getPath(void) const;
  const std::string &getRedirect(void) const;
  const std::string &getCgi(void) const;

  bool isSetAllowed(void) const;

private:
  Type _type;
  const std::string _path;
  Return _return;
  bool _allowSet;
  int _allow;
  std::string _redirect;
};
