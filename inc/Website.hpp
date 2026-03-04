#pragma once

#include "Token.hpp"

class Website {
public:
  Website();
  Website(const Website &);
  Website &operator=(const Website &);
  ~Website();

  Website(std::list<Token>::const_iterator It);

private:
};
