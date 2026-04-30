#include "HttpMethods.hpp"
#include "Logging.hpp"
#include "Request.hpp"
#include <exception>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>

#define MAX_REQUEST 1024

void Request::parseMethod(const std::string &Token)
{
	if (Token == "GET")
		_method = Get;
	else if (Token == "POST")
		_method = Post;
	else if (Token == "DELETE")
		_method = Delete;
	else if (Token == "HEAD")
		_method = Head;
	else
		throw std::runtime_error("invalid Method");
	logging::log(logging::Debug, "parse status_line: got method successfully");
}

void Request::parseResource(const std::string &Token)
{
	if (Token[0] != '/')
		throw std::runtime_error("requestURI not given as absolute path");
	logging::log(logging::Debug, "parse status_line: parse resource successfully");
	size_t pos = Token.find('?');
	if (pos == std::string::npos)
		_resource = Token;
	else{
		_resource = Token.substr(0, pos);
		_queryString = Token.substr(pos + 1);
	}
	pos = _resource.find('.');
	if (pos != std::string::npos)
	{
		char * extension = strdup(_resource.substr(pos).c_str());
		_headers.setContentType(extension);
		logging::log2(logging::Debug, "QueryString is: ", _queryString);
		logging::log2(logging::Debug, "extension of resource is: ", extension);
		free(extension);
	}
}

void Request::parseHttp(const std::string &Token)
{
	if (Token.substr(0, 5) != "HTTP/")
		throw std::runtime_error("invalid HTTP version");
	const size_t tback = Token.size();
	const size_t pos = Token.find('.');
	if (pos == std::string::npos || pos <= 5 || pos == tback - 1 ) //no . or no other char before .
		throw std::runtime_error("no HTTP version given");
	
	std::stringstream smajor(Token.substr(5, pos - 5));
	if (!(smajor >> _majorVersion) || !smajor.eof())
    	throw std::runtime_error("no HTTP version given");

	std::stringstream sminor(Token.substr(pos + 1));
	if (!(sminor >> _minorVersion) || !sminor.eof())
    	throw std::runtime_error("no HTTP version given");

	logging::log(logging::Debug, "parse status_line: parse http version successfully");
}


void Request::parseRequestLine(const std::string &RequestLine)
{
	logging::log(logging::Debug, "parseStatusLine()");
	std::vector<std::string> status = split(RequestLine, " ");
	if (status.size() != 3)
	{
        logging::log(logging::Debug, "Request is invalid after split of StatusLine");
        _state = INVALID;
		return ;
	}
	try
	{
		parseMethod(status[0]);
		parseResource(status[1]);
		parseHttp(status[2]);
		_state = HEADERS;
		return;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << '\n';
        _state = INVALID;
		return ;
	}
}

bool Request::parseRequestLineFromBuffer()
{
    if (_buf.getUsed() == 0)
        return false;
    const std::string s = _buf.getStringFromBuffer();
    const size_t pos = s.find("\r\n");
    if (pos == std::string::npos)
        return false;  // line not complete yet - read more
    const std::string line = s.substr(0, pos);
    parseRequestLine(line);
    _buf.deleteFront(pos + 2);  // remove line + CRLF
    if (_state == INVALID)
        return false; 
    _state = HEADERS;
    return true;
}
