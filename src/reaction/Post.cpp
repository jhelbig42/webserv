#include "Conditions.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"

// think about body is coming in chunks
// maybe Reaction also needs state
void Reaction::initPost(const Request &Req){
	//check if method is allowed -> config file
		//initSendFile(CODE_501, FILE_501);
	//check content-length header is present --> BadRequest
	if (!Req.getHeaders().isSet(HttpHeaders::ContentLength)){
		logging::log(logging::Debug, "Reaction: Content Length Header is not Present");
		initSendFile(CODE_400, FILE_400);
		return;
	}
	logging::log(logging::Debug, "Reaction: Content Length Header is Present");
	//copy buffer if still in buffer
	//create requested file with write access
	
	_buffer	= Req.getBuffer();
	//write into it, what is in buffer
	//check for content length
	//read until done
	//close file
}
