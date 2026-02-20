#pragma once

typedef enum {
  SockRead(1u << 0),
  SockWrite(1u << 1),
  FSockRead(1u << 2),
  FSockWrite(1u << 3),
  NoCond(1u << 4)
} Conditions;

class Connection {
public:
  Connection(const int sock, const sockaddr_storage &addr,
             const socklen_t addr_size);
  ~Connection();

  // getters
  int get_sock(void) const;

  bool serve(const size_t Bytes);
  Conditions getConditions() const;

private:
  Connection(); // should not be possible

  bool _delete = false;

  // serve reads from
  Conditions _conditionsFulfilled;

  int _sock; // client socket fd
  int _sockForward;

  // networking
  struct addrinfo _info;
  struct sockaddr_storage _addr; // client's IP
  socklen_t _addr_size;

  Request _req;
  Response _res;
};
