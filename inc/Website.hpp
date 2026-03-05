#pragma once

#include "Token.hpp"
#include <list>
#include <ostream>

struct Listen {
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
  void addInterface(Listen &Interface);
  const std::list<Listen> &getInterfaces(void) const;

private:
  std::list<Listen> _interfaces; 
};

std::ostream &operator<<(std::ostream &Os, const Listen &If);
std::ostream &operator<<(std::ostream &Os, const Website &site);
