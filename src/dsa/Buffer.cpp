#include "Buffer.hpp"
#include "CompileTimeConstants.hpp"
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
  if (getUsed() == 0 || (getFree() == 0 && getBlocked() != 0))
    format();
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

// to understand this better:
//
// 1) getUsed() is the amount of bytes getting moved when formatting
// i.e. how expensive format() is.
//
// 2) getFree() + getUsed() is the the maximum amount of bytes that can
// be in the buffer after the next call to fill()
//
// 3) getBlocked() is the maximum amount of free bytes that can be made
// available by a call to format()
void Buffer::optimize(const size_t Bytes) {
  // don't format if buffer is almost empty
  if (_start < size / BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_1)
    return;
  const size_type capacity = getFree() + getUsed();
  // don't format if both:
  // 1) formatting is expensive
  // 2) there is enough capacity for the next call to empty()
  if (getUsed() > size / BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2
    && capacity >= Bytes)
    return;
  // don't format if both:
  // 1) there is still a certain amount of capacity in the buffer
  // 2) formatting will not dramatically increase the capacity
  if (capacity > Bytes / BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2
    && capacity + getBlocked()
    <= capacity / BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2)
    return;
  format();
}

void Buffer::reset(void) {
  _start = _end = 0;
}

void Buffer::format(void) {
  memmove(_buffer, _buffer + _start, getUsed());
  _end = getUsed();
  _start = 0;
}

void Buffer::deleteFront(const size_t Bytes) {
  if (Bytes > size || _start > size - Bytes || _start + Bytes > _end)
    reset();
  else
    _start += Bytes;
}
