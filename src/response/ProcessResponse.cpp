#include "Response.hpp"
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <unistd.h>

bool Response::process(const int Socket, const size_t Bytes)
{
  if (_req.getMethod() != Get)
    throw std::runtime_error("Unknown method");
  return processGet(Socket, Bytes);
  // switch (_req.getMethod()) {
    // case Get: return processGet(Socket, Bytes);
    // case Post: return processPost(Socket, Bytes);
    // case Delete: return processDelete(Socket, Bytes);
    // case Head: return processHead(Socket, Bytes);
  // }
}

// bool Response::processHead(const int Socket, const size_t Bytes) {
//   if (!_hasMetadata)
//     return makeMetadata(200);
//   return sendMetadata(Socket, Bytes);
// }

/// still a dummy
bool Response::sendMetadata(const int Socket, const size_t Bytes) {
  return true;
}

bool Response::processGet(const int Socket, const size_t Bytes)
{
  if (!_hasMetadata) {
    _hasMetadata = makeMetadata(200);
    return false;
  }
  if (!_metaDataSent) {
    _metaDataSent = sendMetadata(Socket, Bytes);
    return false;
  }
  if (_bufStart != _bufEnd)
    return sendBuffer(Socket, Bytes);
  return fillBufferFile(Bytes);
}

// bool Response::processDelete(const int Socket, const size_t Bytes)
// {
//   if (!_resourceDeleted)
//     return deleteResource();
//   if (!_hasMetadata)
//     return makeMetadata();
//   return sendMetadata(Socket, Bytes);
// }

bool Response::sendBuffer(const int Socket, const size_t Bytes) {
  ssize_t rc = write(Socket, _buffer + _bufStart, std::min(_bufEnd - _bufStart, Bytes));
  if (rc < 0) {
    int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }  
  _bufStart += (size_t)rc;
  if (_bufStart == _bufEnd)
    _bufStart = _bufEnd = 0;
  return (_eof);
}

// TODO: in case of error, what is the correct action with regards to the client?
bool Response::fillBufferFile(const size_t Bytes) {
  ssize_t rc = read(_fdIn, _buffer, std::min(Bytes, (size_t)_buffSize));
  if (rc < 0) {
    int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }  
  if ((size_t)rc < std::min(Bytes, (size_t)_buffSize))
    _eof = true;
  if (rc == 0)
    return true;
  _bufEnd += (size_t)rc;
  return false;
}
