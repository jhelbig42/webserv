
#include "Logging.hpp"
#include "Request.hpp"
#include <sstream>
#include <string>

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
	const size_t colon = HeaderLine.find(':');
	if (colon == std::string::npos)
	{
		_state = INVALID;
		return;
	}
	const std::string name = HeaderLine.substr(0, colon);
	const std::string value = HeaderLine.substr(colon + 1);
	if (name == "Content-Length")
	{
		size_t length;
		std::stringstream slength(value);
		if (!(slength >> length) || !slength.eof())
		{
			logging::log(logging::Warning, "transform content length into long failed");
			_state = INVALID;
			return;
		}
		_headers.setContentLength(length);
	}
	// unrecognized headers (including gibberish with a colon) are ignored
}
