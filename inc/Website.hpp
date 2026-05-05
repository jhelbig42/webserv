#pragma once

#include <list>
#include <map>
#include <ostream>
#include <string>

struct Listen {
  bool operator==(const Listen &Other) const;
  std::string ip;   // NOLINT(misc-non-private-member-variables-in-classes)
  std::string port; // NOLINT(misc-non-private-member-variables-in-classes)
  void print(std::ostream &Os) const;
};

class PathInfo;
class Location;
class Token;

class Website {
public:
  Website(void);
  Website(const Website &Other);
  explicit Website(const Location &Loc);
  Website &operator=(const Website &Other);
  ~Website(void);
  PathInfo getPathInfo(const std::string &Path) const;
  const std::list<Listen> &getInterfaces(void) const;
  std::ostream &print(std::ostream &Os) const;

private:
  const Location *_website;
};

class PathInfo {
public:
  typedef enum { Default, Return, Cgi } Action;

  PathInfo(void);
  PathInfo(const PathInfo &Other);
  PathInfo &operator=(const PathInfo &Other);
  ~PathInfo(void);

  PathInfo(const Location &Site, const std::string &Path);

  unsigned int getMaxReqBody(void) const;
  const std::string &getRoot(void) const;
  bool getAutoindex(void) const;
  const std::string &getCgiPath(void) const;
  const std::string &getRealPath(void) const;
  int getAllowed(void) const;
  Action getAction(void) const;
  unsigned int getCode(void) const;
  const char *getErrorPage(const unsigned int Code) const;
  void print(std::ostream &Os) const;
  const std::list<std::string> &getIndex(void) const;

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
  std::list<std::string> _index;
  bool _autoindex;
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &Site);
std::ostream &operator<<(std::ostream &Os, const PathInfo &Info);
