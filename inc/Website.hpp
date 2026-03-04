#pragma once

#include "Token.hpp"

struct Listen {
  std::string ip;
  unsigned int port;
};

class Website {
public:
  Website();
  Website(const Website &);
  Website &operator=(const Website &);
  ~Website();

  Website(std::list<Token>::const_iterator It);
  void setIp(const std::string &Ip);

private:
  std::list<Listen> _interfaces; 
};
