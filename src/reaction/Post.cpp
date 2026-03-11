#include "Conditions.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"

#include <fstream>
#include <iostream>
#include <string>

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
	std::string pathname = DEFAULT_PATH + Req.getResource();
	_fdOut = fopen(pathname.c_str(), "w");
	if (!_fdOut)
		initSendFile(CODE_500, NULL);
	logging::log2(logging::Debug, "Reaction: File created for Post Request: ", pathname);
	_processType = ReceiveFile;
  	_conditions = SockRead;
	logging::log(logging::Debug, "Reaction: Post Request initialized successfully, SockRead and ReceiveFile");
}

bool Reaction::receiveFile(const int Socket, const size_t Bytes){
	size_t toReceive = std::min(_reqContLen - _receivedContLen, Bytes);
	if (toReceive > 0)
	{
		ssize_t copied = _buffer.bufToFILE(_fdOut, toReceive);
		if (copied == -1)
			return (false);
		_receivedContLen += static_cast<size_t>(copied);
		_buffer.deleteFront(static_cast<size_t>(copied));
		_buffer.socketToBuf(Socket, Bytes);
		return (false);
	}
	// if we received enough data in comparison to given content length
	logging::log(logging::Debug, "Reaction: Received complete body for Post Request");
	fclose(_fdOut);
	return true;
}
