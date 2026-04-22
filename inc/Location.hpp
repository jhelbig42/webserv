#pragma once

#include "HttpMethods.hpp"
#include <list>
#include <map>
#include <string>

struct ReturnData {
  unsigned int code;
  std::string url;
  ReturnData(void): code(0), url("") {}
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
  void setReturn(const unsigned int Code, const std::string &Url);
  void addAllow(const HttpMethod Method);
  void setRedirect(const std::string &Redirect);
  void setCgi(const std::string &Cgi);
  const std::list<Location> &getLocations(void) const;
  void addLocation(Location &Loc);

  void addErrorPage(const unsigned int Code, const std::string &Path);
  void setRoot(const std::string &RootDir);
  void setMaxReqBody(const unsigned int MaxBody);
  void setAutoindex(const bool IsOn);
  const std::map<unsigned int, std::string> &getErrorPages(void) const;

  const char *getErrorPage(const unsigned int Code) const;
  bool isAllowed(const HttpMethod Method) const;
  int getAllow(void) const;
  const ReturnData &getReturn(void) const;
  const std::string &getPath(void) const;
  const std::string &getRedirect(void) const;
  const std::string &getCgi(void) const;
  const std::string &getRoot(void) const;
  unsigned int getMaxReqBody(void) const;
  bool getAutoindex(void) const;

  bool isSetAutoindex(void) const;
  bool isSetRoot(void) const;
  bool isSetAllow(void) const;
  bool isSetMaxReqBody(void) const;

private:
  typedef enum {
    Interfaces = (1u << 0),
    Root = (1u << 1),
    Autoindex = (1u << 2),
    Allow = (1u << 3),
    MaxReqBody = (1u << 4)
  } SetMembers;

  Type _type;
  int _setMembers;
  std::string _path;
  ReturnData _return;
  int _allow;
  std::string _redirect;
  std::list<Location> _locations; 

  unsigned int _maxReqBody;
  std::string _root;
  std::map<unsigned int, std::string> _errorPages;
  bool _autoindex;
};

std::ostream &operator<<(std::ostream &Os, const Location &Loc);
std::ostream &operator<<(std::ostream &Os, const ReturnData &Ret);
