
bool Response::process(const int Socket, const size_t Bytes)
{
  switch (_method) {
    case Get: return processGet(Socket, Bytes);
    case Post: return processPost(Socket, Bytes);
    case Delete: return processDelete(Socket, Bytes);
    case Head: return processHead(Socket, Bytes);
  }
}

bool Response::processHead(const int Socket, const size_t Bytes) {
  if (!_hasMetadata)
    return makeMetadata();
  return sendMetadata(Socket, Bytes);
}

/// still a dummy
bool Response::makeMetadata(void) {
  _hasMetadata = true;
  return false;
}

/// still a dummy
bool Response::sendMetadata(void) {
  return true;
}

bool Response::processGet(const int Socket, const size_t Bytes)
{
  if (!_hasMetadata)
    return makeMetadata();
  if (!_metaDataSent)
    return sendMetadata(Socket, Bytes);
  if (_bufStart != _bufEnd)
    return sendBuffer(Socket, Bytes);
  return fillBufferFile(Bytes);
}

bool Response::processDelete(const int Socket, const size_t Bytes)
{
  if (!_resourceDeleted)
    return deleteResource();
  if (!_hasMetadata)
    return makeMetadata();
  return sendMetadata(Socket, Bytes);
}

bool Response::sendBuffer(const int Socket, const size_t Bytes) {
  ssize_t rc = write(Socket, _buffer + _bufStart, min(_bufEnd - _bufStart, Bytes));
  if (rc < 0) {
    rc = errno;
    errno = 0;
    throw std::runtimeError(strerror(rc));
  }  
  _bufStart += (size_t)rc;
  if (_bufStart == _bufEnd)
    _bufStart = _bufEnd = 0;
  return (_eof)
}

// TODO: in case of error, what is the correct action with regards to the client?
bool Response::fillBufferFile(const size_t Bytes) {
  ssize_t rc = read(_fdIn, _buffer, min(Bytes, _buffSize));
  if (rc < 0) {
    rc = errno;
    errno = 0;
    throw std::runtimeError(strerror(rc));
  }  
  if (rc < min(Bytes, _buffSize))
    _eof = true;
  if (rc == 0)
    return true;
  _bufEnd += rc;
  return false;
}
