#pragma once

#include "CompileTimeConstants.hpp"

class Buffer {
public:
  Buffer();
  Buffer(const Buffer&);
  Buffer& operator=(const Buffer&);
  ~Buffer();

  const static size_t size = BUFFER_SIZE;

  size_t getBlocked(void) const;
  size_t getUsed(void) const;
  size_t getFree(void) const;

  ssize_t fill(const int Fd, const size_t Bytes);
  ssize_t empty(const int Fd, const size_t Bytes);

private:
  char _buffer[BUFFER_SIZE]
  size_t _start;
  size_t _end;
};

size_t Buffer::getUsed(void) const {
  return _end - _start;
}

size_t Buffer::getBlocked(void) const {
  return _end;
}

size_t Buffer::getFree(void) const {
  return size - _end;
}

ssize_t Buffer::fill(const int Fd, const size_t Bytes) {
  size_t amount = 
}

ssize_t Buffer::empty(const int Fd, const size_t Bytes);
