#pragma once

#include "HttpMethods.hpp"
#include "Location.hpp"
#include "Token.hpp"
#include <list>
#include <map>
#include <ostream>
#include <string>

struct Listen {
  bool operator==(const Listen &Other) const;
  std::string ip;   // NOLINT(misc-non-private-member-variables-in-classes)
  std::string port; // NOLINT(misc-non-private-member-variables-in-classes)
};

class PathInfo;

class Website {
public:
  Website();
  Website(const Website &Other);
  Website &operator=(const Website &Other);
  ~Website();

  explicit Website(std::list<Token>::const_iterator It);
  void addInterface(Listen &If);

  /// \fun addLocation
  /// \brief if a location with the same path already exists it is replaced
  void addLocation(Location &Loc);
  void addErrorPage(const unsigned int Code, const std::string &Path);
  const char *getErrorPage(const unsigned int Code);
  const std::list<Listen> &getInterfaces(void) const;
  const std::list<Location> &getLocations(void) const;
  const std::map<unsigned int, std::string> &getErrorPages(void) const;
  void setRoot(const std::string &RootDir);
  void setAutoindex(const bool IsOn);
  void setMaxReqBody(const unsigned int MaxBody);
  void addAllow(const HttpMethod Method);
  void allowAll(void);
  void allowNone(void);
  const std::string &getRoot(void) const;
  bool getAutoindex(void) const;
  int getAllow(void) const;
  unsigned int getMaxReqBody(void) const;
  bool isSetAutoindex(void) const;
  bool isSetRoot(void) const;
  bool isSetInterfaces(void) const;
  bool isSetAllow(void) const;
  bool isSetMaxReqBody(void) const;

  PathInfo getPathInfo(const std::string &Path) const;

private:
  typedef enum {
    Interfaces = (1u << 0),
    Root = (1u << 1),
    Autoindex = (1u << 2),
    Allow = (1u << 3),
    MaxReqBody = (1u << 4)
  } SetMembers;

  int _setMembers;
  unsigned int _maxReqBody;
  std::list<Listen> _interfaces; 
  std::list<Location> _locations; 
  std::string _root;
  std::map<unsigned int, std::string> _errorPages;
  bool _autoindex;
  int _allow;
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

  const std::string &getCgiPath(void) const;
  const std::string &getRealPath(void) const;
  int getAllowed(void) const;
  Action getAction(void) const;
  unsigned int getCode(void) const;

private:
  void resolveLocations(const std::list<Location> &Locations);
  void populateFromLocation(const Location &Loc);
  std::string _cgiPath;
  std::string _realPath;
  Action _action;
  unsigned int _code;
  int _allow;
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &Site);
std::ostream &operator<<(std::ostream &Os, const PathInfo &Info);
