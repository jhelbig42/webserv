#include "ReasonPhrases.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <algorithm>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

// TODO: Compare data sent with header Content-Length instead of only relying on read() return values

/// \brief attempts to transfer Bytes from FileFd to Socket, using Buf as an intermefiary and to save state
///
/// Expects Buf to not be modified by other parts of the program between calls
///
/// sideffects:
/// calls read on FileFd
/// calls write() on Socket
/// modifies Buf
/// can call close() on FileFd and set it to -1
///
/// \param Socket socket to write to
/// \param FileFd fd to read from (should be a file on disk)
/// \param Buf buffer to be used as an intermediary and to save state between calls
/// \param Bytes amount of bytes which is attempted to be transferred
static bool fileToSocket(const int Socket, int &FileFd, Buffer &Buf, const size_t Bytes);

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
  (void)Socket;
  (void)Bytes;
  return true;
}

bool Response::processGet(const int Socket, const size_t Bytes)
{
  if (!_hasMetadata)
    _hasMetadata = makeMetadata(CODE_200);
  if (!_hasMetadata)
	  return false;
  if (!_metaDataSent) {
    _metaDataSent = sendMetadata(Socket, Bytes);
    return false;
  }
  return dataTransferGet(Socket, _fdIn, _buffer, Bytes);
}

static bool fileToSocket(const int Socket, int &FileFd, Buffer &Buf, const size_t Bytes) {
  if (FileFd >= 0) {
    Buf.optimize(Bytes);
	// edgecase = FileFd empties by exactly filling up Buf. Then empty FileFd would be passed one more time;
	// theory: does not matter as FileFd will spill just 0 next iteration and no exception will be thrown
    if (Buf.fill(FileFd, Bytes) < Bytes
	  && Buf.getFree() > 0
	  && close(FileFd) < 0) {
        logging::log2(logging::Error, "close: ", strerror(errrno));
        errno = 0;
        FileFd = -1;
    }
  }
  const size_t used = Buf.getUsed();
  if (used == 0)
	  return true;
  if (fileFd == -1 && Buf.empty(Socket, Bytes) == used)
	  return true;
  return false;
}

// bool Response::processDelete(const int Socket, const size_t Bytes)
// {
//   if (!_resourceDeleted)
//     return deleteResource();
//   if (!_hasMetadata)
//     return makeMetadata();
//   return sendMetadata(Socket, Bytes);
// }

Response::transferBytes(const int Socket, const size_t Bytes);

/// \brief attempts to send a certain amount of bytes from the buffers content
/// to a socket
///
/// \param Socket to send data to
/// \param Bytes amount of bytes that is maximally read send
/// The actual amount depends on buffer content and Socket's readiness to receive
bool Response::sendBuffer(const int Socket, const size_t Bytes) {
  const ssize_t rc = write(Socket, _buffer + _bufStart, std::min(_bufEnd - _bufStart, Bytes));
  if (rc < 0) {
    const int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }  
  _bufStart += (size_t)rc;
  if (_bufStart == _bufEnd)
    _bufStart = _bufEnd = 0;
  return (_eof);
}

/// \brief reads up to Bytes bytes from file to _buffer
/// actual bytes read are limited by buffer size and the amount of bytes that
/// can be read from file
///
/// sideeffects:
/// usually overwrites part of buffer
/// usually advances _bufEnd
/// exception to this: read() returns <= 0
/// sets _eof to true if nothing more to read from file
///
/// TODO: doublecheck error handling
///
/// \param Bytes amount of bytes that is maximally read into _buffer
/// The actual amount depends on the remaining space in buffer and read().
///
/// \returns true if nothing more to read from file
/// \returns false if still stuff to read from file
bool Response::fillBufferFile(const size_t Bytes) {
  size_t amount = std::min(Bytes, (size_t)buffSize);
  const ssize_t rc = read(_fdIn, _buffer, amount);
  if (rc < 0) {
    const int err = errno;
    errno = 0;
    throw std::runtime_error(strerror(err));
  }  
  if ((size_t)rc < amount) {
    _eof = true;
    return true;
  }
  _bufEnd += (size_t)rc;
  return false;
}
