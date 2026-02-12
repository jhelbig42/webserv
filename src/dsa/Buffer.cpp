Buffer::Buffer(void)
  : _start(0), _end(0) {
}

Buffer::Buffer(const Buffer &other)
  : _start(other._start), _end(other._end) {
  memcpy(_buffer + _start, other._buffer + other._start, other._getUsed());
}

Buffer::Buffer &operator=(const Buffer &other) {
  if (this != &other) {
    _start = other._start;
    _end = other._end;
    memcpy(_buffer + _start, other._buffer + other._start, other._getUsed());
  }
  return *this;
}

Buffer::~Buffer(void) {
}

size_t Buffer::getUsed(void) const {
  return _end - _start;
}

size_t Buffer::getBlocked(void) const {
  return _end;
}

size_t Buffer::getGarbage(void) const {
  return _start;
}

size_t Buffer::getFree(void) const {
  return size - _end;
}

ssize_t Buffer::fill(const int Fd, const size_t Bytes) {
  const size_t amount = std::min(Bytes, getFree());
  const ssize_t rc = recv(Fd, _buffer + _end, MSG_DONTWAIT);
  if (rc < 0)
  {
    int err = errno;
    errno = 0;
    throw std::runtime_error(err);
  }
  _end += rc;
  return rc;
}

ssize_t Buffer::empty(const int Fd, const size_t Bytes) {
  const size_t amount = std::min(Bytes, getUsed());
  const ssize_t rc = send(Fd, _buffer + _start, MSG_NOSIGNAL | MSG_DONTWAIT);
  if (rc < 0)
  {
    int err = errno;
    errno = 0;
    throw std::runtime_error(err);
  }
  _start += rc;
  return rc;
}

void Buffer::optimize(const size_t Bytes) {
  if (getFree() + getUsed() <= Bytes)
    return;
  memmove(_buffer, _buffer + _start, getUsed());
}
