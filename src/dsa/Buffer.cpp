#include "Buffer.hpp"
#include <algorithm>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

/// \brief checks if a file descriptor references a socket
static bool isSocket(const int Fd);

Buffer::Buffer(void) : _start(0), _end(0) {
}

Buffer::Buffer(const Buffer &other) : _start(other._start), _end(other._end) {
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

char &Buffer::operator[](Buffer::size_type i) {
  return _buffer[_start + i];
}

const char &Buffer::operator[](Buffer::size_type i) const {
  return _buffer[_start + i];
}

Buffer::iterator Buffer::begin() {
  return _buffer + _start;
}

Buffer::const_iterator Buffer::begin() const {
  return _buffer + _start;
}

Buffer::iterator Buffer::end() {
  return _buffer + _end;
}

Buffer::const_iterator Buffer::end() const {
  return _buffer + _end;
}

Buffer::size_type Buffer::getUsed(void) const {
  return _end - _start;
}

Buffer::size_type Buffer::getOccupied(void) const {
  return _end;
}

Buffer::size_type Buffer::getBlocked(void) const {
  return _start;
}

Buffer::size_type Buffer::getFree(void) const {
  return size - _end;
}

Buffer::size_type Buffer::fill(const int Fd, const size_t Bytes) {
  const size_t amount = std::min(Bytes, getFree());
  const ssize_t rc = read(Fd, _buffer + _end, amount);
  if (rc < 0) {
    const int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }
  _end += (size_t)rc;
  return (size_t)rc;
}

Buffer::size_type Buffer::empty(const int Fd, const size_t Bytes) {
  const size_t amount = std::min(Bytes, getUsed());
  const ssize_t rc = write(Fd, _buffer + _start, amount);
  if (rc < 0) {
    const int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }
  _start += (size_t)rc;
  if (_start == _end)
    reset();
  return (size_t)rc;
}

void Buffer::optimize(const size_t Bytes) {
  const size_t thresholdDivisor = 8;
  if (_start < size / thresholdDivisor)
    return;
  if (getUsed() > size / thresholdDivisor && getFree() + getUsed() >= Bytes)
    return;
  if (getFree() + getUsed() > Bytes / thresholdDivisor &&
      getFree() + getBlocked() < Bytes)
    return;
  memmove(_buffer, _buffer + _start, getUsed());
  _end = getUsed();
  _start = 0;
}

void Buffer::reset(void) {
  _start = _end = 0;
}
