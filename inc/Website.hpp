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
  void setRoot(const std::string &Root);
  std::string getRoot(void) const;
  bool getAutoindex(void) const;
  void setAutoindex(const bool IsOn);

private:
  bool _autoindex;
  std::list<Listen> _interfaces; 
  std::string _root;
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &site);
