#include "HttpMethods.hpp"
#include "Reaction.hpp"
#include "Request.hpp"

void Reaction::initCGIMethod(const Request &Req){
	if (Req.getMethod() == Post) {
		if (!initPostBody(Req))
			return;
		_processType = CgiPost;
		return;
  	}
	// is CGI but not POST
	// POST has a body to forward; all other methods are immediately done inputwise
	_cgi.setInputDone(true);
	_processType = CgiNotPost;
	//we never write into Socket, just wait to be allowed to read
}
