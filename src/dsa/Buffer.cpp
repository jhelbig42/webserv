#include "Buffer.hpp"
#include "Logging.hpp"
#include <algorithm>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/// \brief checks if a file descriptor references a socket
static bool isSocket(const int Fd);

Buffer::Buffer(void)
  : _start(0), _end(0) {
}

Buffer::Buffer(const Buffer &other)
  : _start(other._start), _end(other._end) {
  memcpy(_buffer + _start, other._buffer + other._start, other.getUsed());
}

Buffer &Buffer::operator=(const Buffer &other) {
  if (this != &other) {
    _start = other._start;
    _end = other._end;
    memcpy(_buffer + _start, other._buffer + other._start, other.getUsed());
  }
  return *this;
}

Buffer::~Buffer(void) {
}

size_t Buffer::getUsed(void) const {
  return _end - _start;
}

size_t Buffer::getOccupied(void) const {
  return _end;
}

size_t Buffer::getBlocked(void) const {
  return _start;
}

size_t Buffer::getFree(void) const {
  return size - _end;
}

ssize_t Buffer::fill(const int Fd, const size_t Bytes) {
  const size_t amount = std::min(Bytes, getFree());
  ssize_t rc;
  if (isSocket(Fd))
    rc = recv(Fd, _buffer + _end, amount, MSG_DONTWAIT);
  else {
    logging::log(logging::Debug, "Buffer::fill: read()");
    rc = read(Fd, _buffer + _end, amount);
    logging::log2(logging::Debug, "Buffer::fill: rc after read(): ", rc);
  }
  if (rc < 0)
  {
    const int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }
  logging::log2(logging::Debug, "Buffer::fill: rc: ", rc);
  _end += (size_t)rc;
  return rc;
}

ssize_t Buffer::empty(const int Fd, const size_t Bytes) {
  logging::log2(logging::Debug, "Buffer::empty: _start: ", _start);
  logging::log2(logging::Debug, "Buffer::empty: _end:   ", _end);
  const size_t amount = std::min(Bytes, getUsed());
  ssize_t rc;
  if (isSocket(Fd))
    rc = send(Fd, _buffer + _start, amount, MSG_DONTWAIT | MSG_NOSIGNAL);
  else
    rc = write(Fd, _buffer + _start, amount);
  if (rc < 0)
  {
    const int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }
  _start += (size_t)rc;
  return rc;
}

void Buffer::optimize(const size_t Bytes) {
  if (getFree() + getUsed() >= Bytes)
    return;
  memmove(_buffer, _buffer + _start, getUsed());
  reset();
}

void Buffer::reset(void) {
  _start = _end = 0;
}

static bool isSocket(const int Fd) {
  struct sockaddr dummy;
  socklen_t len = sizeof(struct sockaddr);
  if (getsockname(Fd, &dummy, &len) == 0)
    return true;
  if (errno != ENOTSOCK) {
    const int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }
  errno = 0;
  return false;
}
