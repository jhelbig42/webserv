#include "HttpMethods.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
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
    	initDelete();
    	return;
  	case Post:
	  	initPost(Req);
	  	return;
  	case Generic:
    	initSendFile(CODE_501, FILE_501);
    	return;
  }
}

void Reaction::initDelete(void) {
  errno = 0;
  if (std::remove(_pathInfo.getRealPath().c_str()) != 0) {
    initError(errno);
    return;
  }
  initSendFile(CODE_202, NULL);
}

void Reaction::initHeadGet(const Request &Req) {
  initSendFile(CODE_200, _pathInfo.getRealPath().c_str());
  if (Req.getMethod() == Get || _fdIn < 0)
    return;
  errno = 0;
  if (close(_fdIn) < 0)
    logging::log2(logging::Error, "close: ", strerror(errno));
  _fdIn = -1;
}


void Reaction::setTmpPathName(void){
	std::stringstream sname;
	sname << _finalPath << _sock;
	_tmpPath = sname.str();
}

//also needs the Post path from config
void Reaction::setFinalPathName(void){
	_finalPath = _pathInfo.getRealPath();
}

void Reaction::initPost(const Request &Req){
	logging::log3(logging::Debug, "Reaction: ", __func__, " called");
	if (!initPostBody(Req))
		return;

	//create requested file with write access
	setFinalPathName();
	setTmpPathName();
	_fdOut = fopen(_tmpPath.c_str(), "w");
	if (!_fdOut)
		initSendFile(CODE_500, NULL);
	logging::log2(logging::Debug, "Reaction: File created for Post Request: ", _tmpPath);
	_processType = ReceiveFile;
	logging::log(logging::Debug, "Reaction: Post Request initialized successfully, SockRead and ReceiveFile");
}



