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
#include <stdexcept>
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
      _cgi.setInputDone(true);
      return true;
    }
    logging::log(logging::Debug, "CGI exited with error code");
	_cgi.setInputDone(true);
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
  if (toSend > 0){
	// see if CGI is still running
    if (checkOnChild() == true) {
		try {
			_buffer.bufToSocket(_cgi.getForwardSocket(), toSend);
			} catch (std::runtime_error &e) {
				// CGI died between checkOnChild() and our write to its socket???
				logging::log3(logging::Info, "sendToCGI: send to CGI failed (",
							  e.what(), "), aborting CGI");
				_cgi.setPid(-1);
				initSendError(CODE_500);
				return;
			}
		}
	}
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
	if (!checkOnChild()) // error already turned into a 500 response
			return;
		if (_cgi.getPid() != -1) // pipe closed (EOF), but exit status not clear yet - try later
			return;
		//here just on EOF, checkOnChild is true and PID set back to -1 from checkOnChild
		//so we are sure exit status was 0
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
  if (_processType == SendFile){
    try{
		return sendFile(Socket, Bytes);
	}
	catch (std::runtime_error &e) {
         // client disconnected between poll() and send()
         logging::log3(logging::Info, "sendToClient: send to client failed (",
                       e.what(), "), closing connection");
         return true;
    }
  }
  if ((_processType == CgiPost || _processType == CgiNotPost) &&
      _cgi.getInputDone()) {
    // logging::log(logging::Debug, "CGI input is done");
    if (sendMetadataIfPending(Socket, Bytes)) {
      return false;
    }
    const size_t used = _buffer.getUsed();
    if (used > 0) {
      ssize_t rc;
	  try {
			rc = _buffer.bufToSocket(Socket, Bytes);
		} catch (std::runtime_error &e) {
         // client disconnected between poll() and send()
         logging::log3(logging::Info, "sendToClient: send to client failed (",
                       e.what(), "), closing connection");
         return true;
       }
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

static bool stringToSocket(const int Socket, std::string &Str,
                           const size_t Bytes) {
  if (Str.empty())
    return true;
  const size_t amount = std::min(Bytes, Str.size());
  errno = 0; 
  const ssize_t rc = send(Socket, Str.c_str(), amount, MSG_DONTWAIT);
  if (rc < 0) {
	throw std::runtime_error(strerror(errno));
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

  ssize_t rc;
  try {
    rc = Buf.bufToSocket(Socket, Bytes);
  } catch (std::runtime_error &e) {
    // client disconnected (e.g. RST) between becoming writable and our send()
    logging::log3(logging::Info, "fileToSocket: send to client failed (",
                  e.what(), "), closing connection");
    return true;
  }
  if (FileFd == -1 && ((rc >= 0) && (size_t)rc == used))
    return true;
  return false;
}
