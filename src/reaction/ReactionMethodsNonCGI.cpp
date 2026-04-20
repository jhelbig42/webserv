#include "Conditions.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PATH "./post"



void Reaction::initMethodNonCGI(const Request &Req) {
  logging::log3(logging::Debug, "Reaction: ", __func__, " called");
  switch (Req.getMethod()) 
  {
  	case Head:
  	case Get:
    	initHeadGet(Req);
    	return;
  	case Delete:
    	initDelete(Req);
    	return;
  	case Post:
	  	initPost(Req);
	  	return;
  	case Generic:
    	initSendFile(CODE_501, FILE_501);
    	return;
  }
}

void Reaction::initDelete(const Request &Req) {
  errno = 0;
  if (std::remove(Req.getResource().c_str()) != 0) {
    initError(errno);
    return;
  }
  initSendFile(CODE_202, NULL);
}

void Reaction::initHeadGet(const Request &Req) {
  initSendFile(CODE_200, Req.getResource().c_str());
  if (Req.getMethod() == Get || _fdIn < 0)
    return;
  errno = 0;
  if (close(_fdIn) < 0)
    logging::log2(logging::Error, "close: ", strerror(errno));
  _fdIn = -1;
}


// think about body is coming in chunks
// maybe Reaction also needs state
void Reaction::initPost(const Request &Req){
	logging::log3(logging::Debug, "Reaction: ", __func__, " called");

	if (!Req.getHeaders().isSet(HttpHeaders::ContentLength)){
		logging::log(logging::Debug, "Reaction: Content Length Header is not Present");
		initSendFile(CODE_400, FILE_400);
		return;
	}
	logging::log(logging::Debug, "Reaction: Content Length Header is Present");
	_reqContLen = Req.getHeaders().getContentLength();
	_receivedContLen = 0;

	//copy buffer if still in buffer
	_buffer	= Req.getBuffer();

	//create requested file with write access
	const std::string pathname = DEFAULT_PATH + Req.getResource();
	_fdOut = fopen(pathname.c_str(), "w");
	if (!_fdOut)
		initSendFile(CODE_500, NULL);
	logging::log2(logging::Debug, "Reaction: File created for Post Request: ", pathname);
	_processType = ReceiveFile;
	logging::log(logging::Debug, "Reaction: Post Request initialized successfully, SockRead and ReceiveFile");
}

void Reaction::initCGIMethod(const Request &Req){
	if (Req.getMethod() == Post) {
    	if (!Req.getHeaders().isSet(HttpHeaders::ContentLength)) {
      		logging::log(logging::Debug, "CGI POST: Content-Length header missing");
      		initSendFile(CODE_400, FILE_400);
      		return;
    	}
    	_reqContLen = Req.getHeaders().getContentLength();
    	_receivedContLen = 0;
    	_buffer = Req.getBuffer();
		_processType = CgiPost;
		return ;
  	}
	// is CGI but not POST
	// POST has a body to forward; all other methods are immediately done inputwise
	_cgi.setInputDone(true);
	_processType = CgiNotPost;
	//we never write into Socket, just wait to be allowed to read
}

