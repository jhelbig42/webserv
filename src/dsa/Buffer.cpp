#include "Buffer.hpp"
#include "CompileTimeConstants.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

char &Buffer::operator[](size_t i) {
  return _buffer[_start + i];
}

const char &Buffer::operator[](size_t i) const {
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

std::string Buffer::getStringFromBuffer(void) const{
  std::string const s(this->begin(), this->end());
  return (s);
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

ssize_t Buffer::fileToBuf(const int Fd, const size_t Bytes) {
  if (getUsed() == size)
    return -1;
  // the following the second part of the or condition could turn out to
  // be a problem in the following case:
  // 1. getBlocked() is small
  // 2. getUsed() is big
  // It could be better to skip filling all together in this case.
  // Probably shoud be handled somewhere else though
  if (getUsed() == 0 || (getFree() == 0 && getBlocked() != 0))
    format();
  const size_t amount = std::min(Bytes, getFree());
  errno = 0;
  const ssize_t rc = read(Fd, _buffer + _end, amount);
  if (rc < 0)
    throw std::runtime_error(strerror(errno));
  _end += (size_t)rc; // safe because rc >= 0 and rc <= getFree()
  return rc;
}

ssize_t Buffer::bufToFile(const int Fd, const size_t Bytes) {
  if (getUsed() == 0)
    return -1;
  const size_t amount = std::min(Bytes, getUsed());
  errno = 0;
  const ssize_t rc = write(Fd, _buffer + _start, amount);
  if (rc < 0)
    throw std::runtime_error(strerror(errno));
  _start += (size_t)rc; // safe because rc >= 0 and rc <= getUsed()
  if (_start == _end)
    reset();
  return rc;
}

ssize_t Buffer::bufToFILE(FILE* file, const size_t Bytes) {
  if (getUsed() == 0)
    return -1;

  const size_t amount = std::min(Bytes, getUsed());

  errno = 0;
  const size_t written = fwrite(_buffer + _start, 1, amount, file);

  if (written < amount && ferror(file))
    throw std::runtime_error(strerror(errno));

  _start += written;

  if (_start == _end)
    reset();

  return (static_cast<ssize_t>(written));
}

ssize_t Buffer::socketToBuf(const int Socket, const size_t Bytes) {
  if (getUsed() == size)
    return -1;
  // the following the second part of the or condition could turn out to
  // be a problem in the following case:
  // 1. getBlocked() is small
  // 2. getUsed() is big
  // It could be better to skip filling all together in this case.
  // Probably shoud be handled somewhere else though
  if (getUsed() == 0 || (getFree() == 0 && getBlocked() != 0))
    format();
  const size_t amount = std::min(Bytes, getFree());
  errno = 0;
  const ssize_t rc = recv(Socket, _buffer + _end, amount, 0);
  if (rc < 0)
    throw std::runtime_error(strerror(errno));
  _end += (size_t)rc; // safe because rc >= 0 and rc <= getFree()
  return rc;
}

ssize_t Buffer::bufToSocket(const int Socket, const size_t Bytes) {
  if (getUsed() == 0)
    return -1;
  const size_t amount = std::min(Bytes, getUsed());
  errno = 0;
  const ssize_t rc = send(Socket, _buffer + _start, amount, MSG_DONTWAIT);
  if (rc < 0)
    throw std::runtime_error(strerror(errno));
  _start += (size_t)rc; // safe because rc >= 0 and rc <= getUsed()
  if (_start == _end)
    reset();
  return rc;
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
  const size_t capacity = getFree() + getUsed();
  // don't format if both:
  // 1) formatting is expensive
  // 2) there is enough capacity for the next call to empty()
  if (getUsed() > size / BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2 &&
      capacity >= Bytes)
    return;
  // don't format if both:
  // 1) there is still a certain amount of capacity in the buffer
  // 2) formatting will not dramatically increase the capacity
  if (capacity > Bytes / BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2 &&
      capacity + getBlocked() <= capacity / BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2)
    return;
  format();
}

void Buffer::reset(void) {
  _start = _end = 0;
}

void Buffer::format(void) {
  if (getUsed() > 0)
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
