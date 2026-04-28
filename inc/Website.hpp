#pragma once

#include "HttpMethods.hpp"
#include "Token.hpp"
#include <list>
#include <map>
#include <ostream>
#include <string>

struct ReturnData {
  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  unsigned int code;
  std::string url;
  // NOLINTEND(misc-non-private-member-variables-in-classes)
  ReturnData(void): code(0), url("") {}
};

struct Listen {
  bool operator==(const Listen &Other) const;
  std::string ip;   // NOLINT(misc-non-private-member-variables-in-classes)
  std::string port; // NOLINT(misc-non-private-member-variables-in-classes)
};

class PathInfo;
class Website;

class Website {
public:
  typedef enum {
    None,
    Cgi,
    Return,
    Redirect
  } Type;

  Website();
  Website(const Website &Other);
  Website &operator=(const Website &Other);
  ~Website();

  explicit Website(const std::string &Path);

  bool getAutoindex(void) const;
  const char *getErrorPage(const unsigned int Code) const;
  unsigned int getMaxReqBody(void) const;
  PathInfo getPathInfo(const std::string &Path) const;

  int getAllow(void) const;
  explicit Website(std::list<Token>::const_iterator It);
  void addInterface(Listen &If);
  /// \fun addLocation
  /// \brief if a location with the same path already exists it is replaced
  void addLocation(Website &Loc);
  void addErrorPage(const unsigned int Code, const std::string &Path);
  const std::list<Listen> &getInterfaces(void) const;
  const std::list<Website> &getLocations(void) const;
  const std::map<unsigned int, std::string> &getErrorPages(void) const;
  void setRoot(const std::string &RootDir);
  void setAutoindex(const bool IsOn);
  void setMaxReqBody(const unsigned int MaxBody);
  void addAllow(const HttpMethod Method);
  void allowAll(void);
  void allowNone(void);
  const std::string &getRoot(void) const;

  const ReturnData getReturn(void) const;
  const std::string getPath(void) const;
  const std::string getRedirect(void) const;
  const std::string getCgi(void) const;
  Type getType(void) const;

  void setReturn(const unsigned int code, const::string &Url);
  void setCgi(const::string &Path);
  void setRedirect(const::string &Path);

  bool isSetAutoindex(void) const;
  bool isSetRoot(void) const;
  bool isSetInterfaces(void) const;
  bool isSetAllow(void) const;
  bool isSetMaxReqBody(void) const;
  bool isRoot(void) const;

private:
  typedef enum {
    Interfaces = (1u << 0),
    Root = (1u << 1),
    Autoindex = (1u << 2),
    Allow = (1u << 3),
    MaxReqBody = (1u << 4)
  } SetMembers;

  bool _isRoot;
  int _setMembers;
  unsigned int _maxReqBody;
  std::list<Listen> _interfaces; 
  std::list<Website> _locations; 
  std::string _root;
  std::map<unsigned int, std::string> _errorPages;
  bool _autoindex;
  int _allow;
  Type _type;
  std::string _path;
  ReturnData _return;
  std::string _redirect;
};

class PathInfo {
public:
  typedef enum {
    Default,
    Return,
    Cgi
  } Action;

  PathInfo(void);
  PathInfo(const PathInfo &Other);
  PathInfo &operator=(const PathInfo &Other);
  ~PathInfo(void);

  PathInfo(const Website &Site, const std::string &Path);

  unsigned int getMaxReqBody(void) const;
  const std::string &getRoot(void) const;
  bool getAutoindex(void) const;
  const std::string &getCgiPath(void) const;
  const std::string &getRealPath(void) const;
  int getAllowed(void) const;
  Action getAction(void) const;
  unsigned int getCode(void) const;
  const char *getErrorPage(const unsigned int Code) const;

private:
  void resolveLocations(const std::list<Location> &Locations);
  void populateFromLocation(const Location &Loc);
  std::string _cgiPath;
  std::string _realPath;
  Action _action;
  unsigned int _code;
  int _allow;

  unsigned int _maxReqBody;
  std::string _root;
  std::list<const std::map<unsigned int, std::string> *> _errorPages;
  bool _autoindex;
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &Site);
std::ostream &operator<<(std::ostream &Os, const PathInfo &Info);
