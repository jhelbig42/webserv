#pragma once

#include "HttpMethods.hpp"
#include <list>
#include <string>

struct ReturnData {
  std::string code;
  std::string url;
};

class Location {
public:
  typedef enum {
    None,
    Cgi,
    Return,
    Redirect
  } Type;

  Location();
  Location(const Location &Other);
  Location &operator=(const Location &Other);
  ~Location();

  explicit Location(const std::string &Path);

  Type getType(void) const;
  void setReturn(const std::string &Code, const std::string &Url);
  void addAllow(const HttpMethod Method);
  void setRedirect(const std::string &Redirect);
  void setCgi(const std::string &Cgi);
  const std::list<Location> &getLocations(void) const;
  void addLocation(Location &Loc);

  bool isAllowed(const HttpMethod Method) const;
  const ReturnData &getReturn(void) const;
  const std::string &getPath(void) const;
  const std::string &getRedirect(void) const;
  const std::string &getCgi(void) const;

  bool isSetAllowed(void) const;

private:
  Type _type;
  std::string _path;
  ReturnData _return;
  bool _allowSet;
  int _allow;
  std::string _redirect;
  std::list<Location> _locations; 
};

std::ostream &operator<<(std::ostream &Os, const Location &Loc);
std::ostream &operator<<(std::ostream &Os, const ReturnData &Ret);
