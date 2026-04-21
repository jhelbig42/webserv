#include "Reaction.hpp"
#include "StatusCodes.hpp"

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