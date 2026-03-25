#include "Conditions.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <stdio.h>
#include <string>
#include <unistd.h>

#define DEFAULT_PATH "./post"
// think about body is coming in chunks
// maybe Reaction also needs state
void Reaction::initPost(const Request &Req){
	logging::log3(logging::Debug, "Reaction: ", __func__, " called");
	//check if method is allowed -> config file
		//initSendFile(CODE_501, FILE_501);
	//check content-length header is present --> BadRequest
	if (!Req.getHeaders().isSet(HttpHeaders::ContentLength)){
		logging::log(logging::Debug, "Reaction: Content Length Header is not Present");
		initSendFile(CODE_400, FILE_400);
		return;
	}
	logging::log(logging::Debug, "Reaction: Content Length Header is Present");
	_reqContLen = Req.getHeaders().getContentLength();

	//copy buffer if still in buffer
	_buffer	= Req.getBuffer();

	//create requested file with write access
	_finalPath = DEFAULT_PATH + Req.getResource();
	_tmpPath = _finalPath + ".tmp";
	_fdOut = fopen(_tmpPath.c_str(), "w");
	if (!_fdOut)
		initSendFile(CODE_500, NULL);
	logging::log2(logging::Debug, "Reaction: File created for Post Request: ", _tmpPath);
	_processType = ReceiveFile;
  	_conditions = SockRead;
	logging::log(logging::Debug, "Reaction: Post Request initialized successfully, SockRead and ReceiveFile");
}

bool Reaction::receiveFile(const int Socket, const size_t Bytes){
	// fill buffer with new data from socket
	_buffer.socketToBuf(Socket, Bytes);
	const size_t toReceive = std::min(_reqContLen - _receivedContLen, Bytes);
	logging::log2(logging::Debug, "Reaction: To receive for Post Request: ", toReceive);
	if (toReceive > 0)
	{
		const ssize_t copied = _buffer.bufToFILE(_fdOut, toReceive);
		if (copied == -1)
			return (false);
		_receivedContLen += static_cast<size_t>(copied);
		_buffer.deleteFront(static_cast<size_t>(copied));
		logging::log3(logging::Debug, "Requested / Received Content Len: ", _reqContLen, _receivedContLen);
	}

	if (!(_receivedContLen == _reqContLen))
		return (false);
	// if we received enough data in comparison to given content length
	logging::log(logging::Debug, "Reaction: Received complete body for Post Request");
	fclose(_fdOut); //close tmp file
	unlink(_finalPath.c_str());
	rename(_tmpPath.c_str(), _finalPath.c_str());
	_buffer.reset();
	initSendFile(CODE_201, NULL);
	return true;
}
