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
  std::string ip;
  std::string port;
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
  void addErrorPage(const unsigned int Code, const std::string &Path);
  const char *getErrorPage(const unsigned int Code);
  const std::list<Listen> &getInterfaces(void) const;
  const std::list<Location> &getLocations(void) const;
  const std::map<unsigned int, std::string> &getErrorPages(void) const;
  void setRoot(const std::string &RootDir);
  void addAllow(const HttpMethod Method);
  std::string getRoot(void) const;
  bool getAutoindex(void) const;
  int getAllow(void) const;
  void setAutoindex(const bool IsOn);
  bool isSetAutoindex(void) const;
  bool isSetRoot(void) const;
  bool isSetInterfaces(void) const;
  bool isSetLocations(void) const;
  bool isSetAllow(void) const;

private:
  typedef enum {
    Interfaces = (1u << 0),
    Root = (1u << 1),
    Autoindex = (1u << 2),
    Allow = (1u << 3),
    Locations = (1u << 4)
  } SetMembers;

  int _setMembers;
  std::list<Listen> _interfaces; 
  std::list<Location> _locations; 
  std::string _root;
  std::map<unsigned int, std::string> _errorPages;
  bool _autoindex;
  int _allow;
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &Site);
