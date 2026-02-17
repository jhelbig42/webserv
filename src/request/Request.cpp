#include "Request.hpp"

// Source - https://stackoverflow.com/a/14266139
// Posted by Vincenzo Pii, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-11, License - CC BY-SA 4.0

Request::Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid)
  : _method(Method), _resource(Resource), _majorVersion(MajorV), _minorVersion(MinorV), _valid(Valid) {}


Request::Request(const char *input)
	: _method(Generic), _resource(""), _majorVersion(0), _minorVersion(0), _valid(false)
{
	this->parse(input, 100);
}  

std::vector<std::string> split(const std::string& s, const std::string& delimiter) 
{
    std::vector<std::string> tokens;
    std::string token;
	size_t last = 0;
	size_t next = 0;
    while ((next = s.find(delimiter, last)) != std::string::npos) {
        token = s.substr(last, next - last);
        tokens.push_back(token);
		last = next + delimiter.length();
    }
    tokens.push_back(s.substr(last)); // last string added here
	logging::log("parse status_line: split Successfull", logging::Debug);
    return tokens;
}

void Request::parse_method(std::string token)
{
	if (token == "GET")
		this->_method = Get;
	else if (token == "POST")
		this->_method = Post;
	else if (token == "DELETE")
		this->_method = Delete;
	else
		throw std::runtime_error("invalid Method");
	logging::log("parse status_line: got method successfully", logging::Debug);
}

void Request::parse_resource(std::string token)
{
	if (token[0] != '/')
		throw std::runtime_error("requestURI not given as absolute path");
	this->_resource = token;
	logging::log("parse status_line: parse resource successfully", logging::Debug);
}
void Request::parse_http(std::string token)
{
	if (token.substr(0, 5) != "HTTP/")
		throw std::runtime_error("invalid HTTP version");
	size_t tback = token.size();
	size_t pos = token.find(".");
	if (pos == std::string::npos || pos <= 5 || pos == tback - 1 ) //no . or no other char before .
		throw std::runtime_error("no HTTP version given");
	std::string smajor = token.substr(5, pos - 5);
	int major = atoi(smajor.c_str());
	std::string sminor = token.substr(pos + 1, tback - pos - 1);
	int minor = atoi(sminor.c_str());
	if ((major == 0 && smajor != "0") || (minor == 0 && sminor != "0") || major < 0 || minor < 0)
		throw std::runtime_error("no HTTP version given");
	this->_majorVersion = major;
	this->_minorVersion = minor;
	logging::log("parse status_line: parse http version successfully", logging::Debug);
}

//   GET /TheProject.html HTTP/1.0
void Request::parse(const char *buffer, const size_t bytes)
{
	//assuming I am getting just the status line

	//correct amount of SP? ; always just one space 
	std::vector<std::string> status = split(buffer, " ");
	//status line with exactly 3 tokens
	if (status.size() != 3)
	{
		this->_valid =false;
		return ;
	}
	try
	{
		parse_method(status[0]);
		parse_resource(status[1]);
		parse_http(status[2]);
		this->_valid = true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		this->_valid = false;
		throw (e);
		return ;
	}
	(void)bytes;
}

bool Request::isValid() const {
	return _valid;
}

size_t Request::getMajorV() const {
	return _majorVersion;
}

size_t Request::getMinorV() const {
	return _minorVersion;
}

const std::string &Request::getResource() const {
	return _resource;
}

HttpMethod Request::getMethod() const {
	return _method;
}
