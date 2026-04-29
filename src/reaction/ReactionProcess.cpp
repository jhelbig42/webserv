#include "Reaction.hpp"

#include "Buffer.hpp"
#include "Conditions.hpp"
#include "Logging.hpp"
#include "StatusCodes.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
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

bool Reaction::process(const int Socket, const size_t Bytes, const int Condition){
  
  if (!checkOnChild())
    return false;
 // we just polled for what we need
  if (Condition & FSockRead)   
    receiveFromCGI(Bytes);
  else if (Condition & FSockWrite)  
    sendToCGI(Bytes);
  else if (Condition & SockRead)    
    recvFromClient(Socket, Bytes);
  else if (Condition & SockWrite)   
    return sendToClient(Socket, Bytes);
  return false;
}

bool Reaction::checkOnChild(void){
	
	const pid_t pid = _cgi.getPid();
	if (pid == -1) // no CGI
		return true;
  
  	int status;
	const pid_t result = waitpid(pid, &status, WNOHANG);
	
  	if (result == -1){
    	_cgi.setPid(-1);
		initSendFile(CODE_500, getErrorFile(CODE_500).c_str());
		return false; // waitpid failed => internal server error
	}

	if (result == 0) // child not finished yet
		return true; //come back later
	
	if (WIFEXITED(status)) { //child somehow exited
		// find out how
    	if (WEXITSTATUS(status) == 0) {
      		logging::log(logging::Debug, "CGI exited normally with 0");
			_cgi.setPid(-1);
      		return true;
    	}
    	logging::log(logging::Debug, "CGI exited with error code");
    } 
	else if (WIFSIGNALED(status)) {
        logging::log(logging::Debug, "CGI was killed by signal");
    }
    _cgi.setPid(-1);
    initSendFile(CODE_500, getErrorFile(CODE_500).c_str());
    return false;
}

void Reaction::sendToCGI(const size_t Bytes){
  if (_processType != CgiPost || _cgi.isInputDone())
  	return; // should never happen

  logging::log(logging::Debug, "sendToCGI");

  // forward buffered body data to the CGI process, max Bytes bytes 
  // TO DO: is it necessary to limit it to Bytes bytes? because it is a server internal process
  // only what is in the buffer can be send. As we just read Bytes bytes into in, it cannot execeed Bytes bytes
  const size_t toSend = std::min(_reqContLen - _receivedContLen, _buffer.getUsed());
  if (toSend > 0) {
    const ssize_t sent = _buffer.bufToSocket(_cgi.getForwardSocket(), toSend);
    if (sent > 0) {
      _receivedContLen += static_cast<size_t>(sent);
      //_buffer.deleteFront(static_cast<size_t>(sent));
    }
  }

  // once the full body is forwarded, mark input as done
  // the CGI script is expected to use CONTENT_LENGTH to know how many bytes to read
  if (_receivedContLen >= _reqContLen) {
    logging::log(logging::Debug, "sendToCGI: body fully forwarded to CGI");
    _cgi.setInputDone(true);
    _buffer.reset();
  }
}

void Reaction::receiveFromCGI(const size_t Bytes){
	if ((_processType != CgiPost && _processType != CgiNotPost)
		|| !_cgi.isInputDone())
			return;
	logging::log(logging::Debug, "sendfromCGI");

	// fill buffer from CGI socket — FSockRead guarantees data is available
	_buffer.optimize(Bytes);
	const ssize_t rc = _buffer.fileToBuf(_cgi.getForwardSocket(), Bytes);
	if (rc < 0){ // when buffer is full
		return ;
	}
	if (rc == 0) {
		// EOF from forwardSocket transition to SendFile from remaining buffer
		_fdIn = -1;
		_processType = SendFile;
	}
}

void Reaction::recvFromClient(const int Socket, const size_t Bytes) {
  if (_processType == ReceiveFile)
    receiveBodyIntoServerFile(Socket, Bytes);
  else if (_processType == CgiPost && !_cgi.isInputDone())
    receiveBodyIntoServerBuffer(Socket, Bytes);
}

bool Reaction::sendToClient(const int Socket, const size_t Bytes) {
  if (_processType == SendFile)
    return sendFile(Socket, Bytes);
  if ((_processType == CgiPost || _processType == CgiNotPost) 
  		&& _cgi.isInputDone()) 
  {
    if (sendMetadataIfPending(Socket, Bytes))
      return false;
	const size_t used = _buffer.getUsed();
    if (used > 0)
	{
       const ssize_t rc = _buffer.bufToSocket(Socket, Bytes);
	    if ((rc >= 0) && (size_t)rc == used)
  			return true;
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
  if (sendMetadataIfPending(Socket, Bytes))
    return false;
  return fileToSocket(Socket, _fdIn, _buffer, Bytes);
}

void Reaction::receiveBodyIntoServerBuffer(const int Socket, const size_t Bytes){
	// fill buffer with new data from socket
	const size_t toReceive = std::min(_reqContLen - _receivedContLen, Bytes);
	logging::log2(logging::Debug, "Reaction: To receive for CGI Post Request: ", toReceive);
	try {
		const ssize_t received = _buffer.socketToBuf(Socket, toReceive);
		if (received == -1) //not possible to read anything into the buffer
			return ; //means we are just done for this round
		_receivedContLen += static_cast<size_t>(received);
		logging::log3(logging::Debug, "Requested / Received Content Len: ", _reqContLen, _receivedContLen);
		if (_receivedContLen == _reqContLen)
			_cgi.setInputDone(true);
	}
	catch (std::runtime_error &){
		initSendFile(CODE_500, getErrorFile(CODE_500).c_str());
		return ;
	}
	return ;
}

void Reaction::receiveBodyIntoServerFile(const int Socket, const size_t Bytes){
	// fill buffer with new data from socket
	const size_t toReceive = std::min(_reqContLen - _receivedContLen, Bytes);
	logging::log2(logging::Debug, "Reaction: To receive for Post Request: ", toReceive);
	try {
		const ssize_t received = _buffer.socketToBuf(Socket, toReceive);
		logging::log2(logging::Debug, "Reaction: read from Client into ServerBuffer: ", received);
		if (received == -1) //not possible to read anything into the buffer
			return ; //means we are just done
	}
	catch (std::runtime_error &){
		fclose(_fdOut);
		unlink(_tmpPath.c_str());
		initSendFile(CODE_500, getErrorFile(CODE_500).c_str());
		return ;
	}

	if (toReceive > 0)
	{
		const ssize_t copied = _buffer.bufToFile(fileno(_fdOut), toReceive);
		logging::log2(logging::Debug, "Reaction: copied from Server Buffer into File: ", copied);
		if (copied == -1)
			return ;
		_receivedContLen += static_cast<size_t>(copied);
		logging::log3(logging::Debug, "Requested / Received Content Len: ", _reqContLen, _receivedContLen);
	}

	if (!(_receivedContLen == _reqContLen))
		return ;
	// if we received enough data in comparison to given content length
	logging::log(logging::Debug, "Reaction: Received complete body for Post Request");
	fclose(_fdOut);
	unlink(_finalPath.c_str()); // fails if file does not exist, but we do not care
	if (rename(_tmpPath.c_str(), _finalPath.c_str())){ //rename the just closed file
		logging::log(logging::Debug, "Reaction: Renaming failed - should never happen");
		initSendFile(CODE_500, getErrorFile(CODE_500).c_str());
		return;
	}
	logging::log(logging::Debug, "Reaction: Renaming successfull");
	_buffer.reset();
	initSendFile(CODE_201, NULL);
	return ;
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
  if ((size_t)rc == amount) // cast is safe because rc > 0
    return true;
  Str.erase(0, (size_t)rc); // cast is safe because rc > 0
  return false;
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
