#pragma once

class Listen {
public:
  typedef enum { Ipv4, Ipv6 } Type;
  std::string ip;
  Type type;
  unsigned int port;
  int socket;
};

class RedirectRule {
public:
  RedirectRule(const std::string &From, const std::string &To);
  std::string resolve(const std::string &Orig) const;
  bool applicable(const std::string &Orig) const!
  unsigned int getPrio(void) const;

private:
  const std::string _from;
  const std::string _to;
}

RedirectRule::RedirectRule(const std::string &From, const std::string &To)
  : _from(From), _to(To) {
}

unsigned int RedirectRule::getPrio(void) const {
  if (*_from.begin() == '*')
    return INT_MAX + from.length();
  return from.length();
}

// DUMMY
bool applicable(const std::string &Orig) const {
  return true;
}

class Redirects {
  std::string resolve(const std::string &Orig) const;

private:
  std::list<RedirectRule> rules;
}

class Website {
public:
  const char *getErrorPage(const int Code) const;
  std::string realResource(const std::string &Orig) const; 
  Cgi getCgi(const std::string &Resource);

private:
  std::list<Listen> _interfaces;
  std::string _root;
  std::string _default;
  int _allowed; // flags
  Redirects _redirects;
}

class Resource {
  const std::string &getString(void) const;
  const std::string &getProgram(void) const;
  
}
