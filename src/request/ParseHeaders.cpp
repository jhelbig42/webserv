
#include "Logging.hpp"
#include "Request.hpp"
#include <string>
#include <sstream>
#include <sys/types.h>
#include <vector>

#define MAX_REQUEST 1024

bool Request::parseHeadersFromBuffer()
{
    if (_buf.getUsed() == 0)
        return false;
    const std::string s = _buf.getStringFromBuffer();
    const size_t pos = s.find("\r\n");
    if (pos == std::string::npos)
        return false;  // header line not complete
    if (pos == 0)// buffer was "\r\n\r\n"
    {
        _buf.deleteFront(2);  // remove second "\r\n"
        _state = COMPLETE;
        return true;
    }
    const std::string headerLine = s.substr(0, pos);
    parseHeader(headerLine);
	_buf.deleteFront(pos + 2);
	if (_state == INVALID)
		return false;
    return true;
}

// RFC: 
// Each header field consists
 //  of a name followed immediately by a colon (":"), a single space (SP)
 //  character, and the field value.
void Request::parseHeader(const std::string &HeaderLine)
{
	logging::log(logging::Debug, "parseHeaderLine()");
	std::vector<std::string> token = split(HeaderLine, ": ");
	if (token.size() != 2) 
	{
		_state = INVALID; // request is made invalid on every headerline that is syntactically incorrect
		return ;
	}
    if (token[0] == "Content-Length")
    {
        size_t length;
        std::stringstream slength(token[1]);
        if (!(slength >> length) || !slength.eof())
        {
            logging::log(logging::Warning, "transform content length into long failed");
            _state = INVALID;
            return;
        }
        _headers.setContentLength(length);   
    }
    // headers that are syntactically correct, are just skipped over
}
