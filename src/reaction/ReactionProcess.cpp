#include "Reaction.hpp"

#include "Buffer.hpp"
#include "Conditions.hpp"
#include "Logging.hpp"
#include "NetworkingDefines.hpp"
#include "StatusCodes.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/// \brief reads data from a file descriptor and writes it to a socket
///
/// A buffer is used to save the state of the transmission between calls.
/// The buffer should not be changed between calls until the transmission
/// is done.
///
/// side effects:
/// Reads from FileFd
/// Writes to Socket
/// modifies Buf
/// If there is nothing more to read from FileFd closes it and sets it to -1
/// Logs and error if closing FileFd fails
///
/// \param Socket socket to send data to
/// \param FileFd file to read data from, will be set to -1 if nothing more to
/// read \param Buf buffer which is used to save the transmission state between
/// calls \param Bytes amount of bytes which is attempted to be transmitted
///
/// \returns true if transmission is completed
/// \returns false if transmission is not completed
static bool fileToSocket(const int Socket, int &FileFd, Buffer &Buf,
                         const size_t Bytes);

/// \brief writes the content of an std::string object to a socket
///
/// side effects:
/// if the entire string Str can't be sent
/// it will be modified to represent the unsent data
/// Writes to Socket
///
/// \returns true if transmission is completed
/// \returns false if transmission is not completed
static bool stringToSocket(const int Socket, std::string &Str,
                           const size_t Bytes);

bool Reaction::process(const int Socket, const size_t Bytes,
                       const int Condition) {

  if (!checkOnChild())
    return false;
  logging::log2(logging::Debug, "child finished for socket ", Socket);
  // we just polled for what we need
  if (Condition & FSockRead)
    receiveFromCGI(Bytes);
  if (Condition & FSockWrite)
    sendToCGI(Bytes);
  if (Condition & SockRead)
    recvFromClient(Socket, Bytes);
  if (Condition & SockWrite)
    return sendToClient(Socket, Bytes);
  return false;
}

bool Reaction::checkOnChild(void) {
  const pid_t pid = _cgi.getPid();
  if (pid == -1) // no CGI
    return true;
  time_t timeElapsed = time(NULL) - _cgi.getTimeLastActive();
  if (timeElapsed > CGI_TIMEOUT) {
    logging::log(logging::Debug, "CGI timed out.");
    kill(pid, SIGKILL);
    _cgi.setPid(-1);
    initSendError(CODE_500);
    return false;
  }
  int status;
  const pid_t result = waitpid(pid, &status, WNOHANG);
  if (result == -1) {
    _cgi.setPid(-1);
    initSendError(CODE_500);
    return false; // waitpid failed => internal server error
  }
  if (result == 0)         // child not finished yet
    return true;           // come back later
  if (WIFEXITED(status)) { // child somehow exited
    _cgi.setChildProcessDone(true);
    // find out how
    if (WEXITSTATUS(status) == 0) {
      logging::log(logging::Debug, "CGI exited normally with 0");
      _cgi.setPid(-1);
      return true;
    }
    logging::log(logging::Debug, "CGI exited with error code");
  } else if (WIFSIGNALED(status)) {
    logging::log(logging::Debug, "CGI was killed by signal");
  }
  _cgi.setPid(-1);
  initSendError(CODE_500);
  return false;
}

void Reaction::sendToCGI(const size_t Bytes) {
  (void)Bytes;
  if (_processType != CgiPost || _cgi.getInputDone())
    return; // should never happen
  logging::log(logging::Debug, "sendToCGI");
  // forward whatever is buffered — _buffer only holds data received but not yet
  // forwarded
  const size_t toSend = _buffer.getUsed();
  if (toSend > 0)
    _buffer.bufToSocket(_cgi.getForwardSocket(), toSend);
  // mark input done only once the full body is both received from the client
  // and drained from the buffer to CGI
  if (_receivedContLen >= _reqContLen && _buffer.getUsed() == 0) {
    logging::log(logging::Debug, "sendToCGI: body fully forwarded to CGI");
    _cgi.setInputDone(true);
  }
}

void Reaction::receiveFromCGI(const size_t Bytes) {
  if ((_processType != CgiPost && _processType != CgiNotPost) ||
      !_cgi.getInputDone())
    return;

  // fill buffer from CGI socket — FSockRead guarantees data is available
  _buffer.optimize(Bytes);
  // logging::log2(logging::Debug, "receiveFromCGI - receiving from fd: ",
  // _cgi.getForwardSocket());
  const ssize_t rc = _buffer.fileToBuf(_cgi.getForwardSocket(), Bytes);
  // const ssize_t rc = _buffer.fileToBuf(5, Bytes);
  if (rc < 0) { // when buffer is full
    return;
  }
  if (rc == 0) {
    // EOF from forwardSocket transition to SendFile from remaining buffer
    _fdIn = -1;
    _processType = SendFile;
  } else {
    _cgi.setTimeLastActive(time(NULL));
  }
}

void Reaction::recvFromClient(const int Socket, const size_t Bytes) {
  if (_processType == CgiPost && !_cgi.getInputDone())
    receiveBodyIntoServerBuffer(Socket, Bytes);
}

bool Reaction::sendToClient(const int Socket, const size_t Bytes) {
  // logging::log(logging::Debug, "Reaction::sendToClient()");
  if (_processType == SendFile)
    return sendFile(Socket, Bytes);
  if ((_processType == CgiPost || _processType == CgiNotPost) &&
      _cgi.getInputDone()) {
    // logging::log(logging::Debug, "CGI input is done");
    if (sendMetadataIfPending(Socket, Bytes)) {
      return false;
    }
    const size_t used = _buffer.getUsed();
    if (used > 0) {
      const ssize_t rc = _buffer.bufToSocket(Socket, Bytes);
      if ((rc >= 0) && (size_t)rc == used &&
          (_processType == ReceiveFile || _cgi.getChildProcessDone())) {
        logging::log2(logging::Debug, __func__, " returns true");
        return true;
      }
      return false;
    }
    return false;
  }
  return false; // should never be reached
}

bool Reaction::sendMetadataIfPending(const int Socket, const size_t Bytes) {
  if (_metadataSent)
    return false;
  _metadataSent = stringToSocket(Socket, _metadata, Bytes);
  return true;
}

bool Reaction::sendFile(const int Socket, const size_t Bytes) {
  if (sendMetadataIfPending(Socket, Bytes)) {
    return false;
  }
  if (!_body.empty())
    return stringToSocket(Socket, _body, Bytes);
  return fileToSocket(Socket, _fdIn, _buffer, Bytes);
}

void Reaction::receiveBodyIntoServerBuffer(const int Socket,
                                           const size_t Bytes) {
  // fill buffer with new data from socket
  const size_t toReceive = std::min(_reqContLen - _receivedContLen, Bytes);
  logging::log2(logging::Debug,
                "Reaction: To receive for CGI Post Request: ", toReceive);
  try {
    const ssize_t received = _buffer.socketToBuf(Socket, toReceive);
    if (received == -1) // not possible to read anything into the buffer
      return;           // means we are just done for this round
    _receivedContLen += static_cast<size_t>(received);
    logging::log3(logging::Debug,
                  "Requested / Received Content Len: ", _reqContLen,
                  _receivedContLen);
  } catch (std::runtime_error &) {
    initSendError(CODE_500);
    return;
  }
  return;
}

// TODO: catching SIGPIPE still missing
// TODO: handle rc < 0
// TODO: doublecheck error handling
static bool stringToSocket(const int Socket, std::string &Str,
                           const size_t Bytes) {
  if (Str.empty())
    return true;
  const size_t amount = std::min(Bytes, Str.size());
  const ssize_t rc = send(Socket, Str.c_str(), amount, MSG_DONTWAIT);
  if (rc < 0) {
    logging::log3(logging::Error, __func__, ": ", strerror(errno));
    return false;
  }
  Str.erase(0, (size_t)rc); // cast is safe because rc > 0
  return Str.empty();
}

// for ideas about performance improvement check comments to implementations
// of Buffer::fill()
static bool fileToSocket(const int Socket, int &FileFd, Buffer &Buf,
                         const size_t Bytes) {
  if (FileFd >= 0) {
    Buf.optimize(Bytes);
    // edgecase = FileFd empties by exactly filling up Buf. Then empty FileFd
    // would be passed one more time; theory: does not matter as FileFd will
    // spill just 0 next iteration and no exception will be thrown
    const ssize_t rc = Buf.fileToBuf(FileFd, Bytes);
    if ((size_t)rc < Bytes && Buf.getFree() > 0) {
      if (close(FileFd) < 0)
        logging::log2(logging::Error, "close: ", strerror(errno));
      FileFd = -1;
    }
  }
  const size_t used = Buf.getUsed();
  if (used == 0) // nothing read from FileFd and no residue from last time
    return true;
  const ssize_t rc = Buf.bufToSocket(Socket, Bytes);
  if (FileFd == -1 && ((rc >= 0) && (size_t)rc == used))
    return true;
  return false;
}
