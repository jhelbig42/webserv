#pragma once

#include "Token.hpp"
#include <list>
#include <ostream>

struct Listen {
  bool operator==(const Listen &other) const;
  std::string ip;
  std::string port;
};

class Website {
public:
  Website();
  Website(const Website &);
  Website &operator=(const Website &);
  ~Website();

  Website(std::list<Token>::const_iterator It);
  void addInterface(Listen &If);
  const std::list<Listen> &getInterfaces(void) const;
  void setRoot(const std::string &RootDir);
  std::string getRoot(void) const;
  bool getAutoindex(void) const;
  void setAutoindex(const bool IsOn);
  bool isSetAutoindex(void) const;
  bool isSetRoot(void) const;
  bool isSetInterfaces(void) const;

private:
  typedef enum {
    Interfaces = (1u << 0),
    Root = (1u << 1),
    Autoindex = (1u << 2)
  } SetMembers;

  int _setMembers;
  std::list<Listen> _interfaces; 
  std::string _root;
  bool _autoindex;
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &site);
