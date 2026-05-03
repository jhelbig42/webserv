#pragma once

#include "HttpMethods.hpp"
#include "Token.hpp"
#include "Website.hpp"
#include <ostream>

class Location {
public:
  typedef enum { None, Cgi, Return, Redirect } Type;

  Location();
  Location(const Location &Other);
  Location &operator=(const Location &Other);
  ~Location();

  explicit Location(const std::string &Path);

  bool getAutoindex(void) const;
  const char *getErrorPage(const unsigned int Code) const;
  unsigned int getMaxReqBody(void) const;
  PathInfo getPathInfo(const std::string &Path) const;

  int getAllow(void) const;
  explicit Location(std::list<Token>::const_iterator It);
  void addInterface(Listen &If);
  /// \fun addLocation
  /// \brief if a location with the same path already exists it is replaced
  void addLocation(Location &Loc);
  void addErrorPage(const unsigned int Code, const std::string &Path);
  const std::list<Listen> &getInterfaces(void) const;
  const std::list<Location> &getLocations(void) const;
  const std::list<std::string> &getIndex(void) const;
  const std::map<unsigned int, std::string> &getErrorPages(void) const;
  void setRoot(const std::string &RootDir);
  void setAutoindex(const bool IsOn);
  void setMaxReqBody(const unsigned int MaxBody);
  void addAllow(const HttpMethod Method);
  void addIndex(const std::string &Resource);
  void allowAll(void);
  void allowNone(void);
  void setAsRoot(void);
  const std::string &getRoot(void) const;

  unsigned int getReturnCode(void) const;
  const std::string &getReturnPath(void) const;

  const std::string &getPath(void) const;
  const std::string &getRedirect(void) const;
  const std::string &getCgi(void) const;
  Type getType(void) const;

  void setReturn(const unsigned int Code, const ::std::string &Url);
  void setCgi(const ::std::string &Path);
  void setRedirect(const ::std::string &Path);

  std::ostream &print(std::ostream &Os) const;

  bool isSetAutoindex(void) const;
  bool isSetRoot(void) const;
  bool isSetInterfaces(void) const;
  bool isSetAllow(void) const;
  bool isSetMaxReqBody(void) const;
  bool isRoot(void) const;
  bool isSetIndex(void) const;

private:
  typedef enum {
    Interfaces = (1u << 0),
    Root = (1u << 1),
    Autoindex = (1u << 2),
    Allow = (1u << 3),
    MaxReqBody = (1u << 4),
    Index = (1u << 5)
  } SetMembers;

  bool _isRoot;
  int _setMembers;
  unsigned int _maxReqBody;
  std::list<Listen> _interfaces;
  std::list<Location> _locations;
  std::list<std::string> _index;
  std::string _root;
  std::map<unsigned int, std::string> _errorPages;
  bool _autoindex;
  int _allow;
  Type _type;
  std::string _path;
  std::string _redirect;
  unsigned int _returnCode;
  std::string _returnPath;
};
