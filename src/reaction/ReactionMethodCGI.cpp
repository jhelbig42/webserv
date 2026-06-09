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
	_cgi.setInputDone(true);
	_processType = CgiNotPost;
}
