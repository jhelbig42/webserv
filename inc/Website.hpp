#pragma once

#include "HttpMethods.hpp"
#include "Token.hpp"
#include <list>
#include <ostream>

struct Listen {
  bool operator==(const Listen &other) const;
  std::string ip;
  std::string port;
};

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

class Website {
public:
  Website();
  Website(const Website &);
  Website &operator=(const Website &);
  ~Website();

  explicit Website(std::list<Token>::const_iterator It);
  void addInterface(Listen &If);

  /// \fun addLocation
  /// \brief if a location with the same path already exists it is replaced
  void addLocation(Location &Loc);
  const std::list<Listen> &getInterfaces(void) const;
  const std::list<Location> &getLocations(void) const;
  void setRoot(const std::string &RootDir);
  void addAllow(const HttpMethod Method);
  std::string getRoot(void) const;
  bool getAutoindex(void) const;
  int getAllow(void) const;
  void setAutoindex(const bool IsOn);
  bool isSetAutoindex(void) const;
  bool isSetRoot(void) const;
  bool isSetInterfaces(void) const;
  bool isSetAllow(void) const;

private:
  typedef enum {
    Interfaces = (1u << 0),
    Root = (1u << 1),
    Autoindex = (1u << 2),
    Allow = (1u << 3)
  } SetMembers;

  int _setMembers;
  std::list<Listen> _interfaces; 
  std::list<Location> _locations; 
  std::string _root;
  bool _autoindex;
  int _allow;
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &Site);
std::ostream &operator<<(std::ostream &Os, const Location &Loc);
std::ostream &operator<<(std::ostream &Os, const Return &Ret);
