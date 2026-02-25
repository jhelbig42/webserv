#include "Request.hpp"
#include "Logging.hpp"
#include "NetworkingDefines.hpp"

#define MAX_REQUEST 1024

bool Request::parseHeadersFromBuffer()
{
    if (_buf.getUsed() == 0)
        return false;
    std::string s(_buf.begin(), _buf.end());
    size_t pos = s.find("\r\n");
    if (pos == std::string::npos)
        return false;  // header line not complete
    if (pos == 0)// buffer was "\r\n\r\n"
    {
        _buf.deleteFront(2);  // remove second "\r\n"
        _state = COMPLETE;
        return true;
    }
    std::string headerLine = s.substr(0, pos);

    //parseHeader(headerLine);

    _buf.deleteFront(pos + 2);
    return true;
}
// 
void Request::parseHeader(std::string HeaderLine)
{
	logging::log(logging::Debug, "parseStatusLine()");
	std::vector<std::string> status = split(HeaderLine, ": ");
	if (status.size() == 1) 
	{
		_valid = false;
		return ;
	}
	try
	{

		_valid = true; // if we are here statusline set _valid to true already
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return ;
	}
}