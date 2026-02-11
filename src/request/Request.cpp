#include "Request.hpp"

// Source - https://stackoverflow.com/a/14266139
// Posted by Vincenzo Pii, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-11, License - CC BY-SA 4.0

std::vector<std::string> split(const std::string& s, const std::string& delimiter) 
{
    std::vector<std::string> tokens;
    std::string token;
	size_t last = 0;
	size_t next = 0;
    while ((next = s.find(delimiter)) != std::string::npos) {
        token = s.substr(last, next - last);
        tokens.push_back(token);
		last = next + 1;
    }
	token = s.substr(last, s.back());
    tokens.push_back(s); // last string added here

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
	{
		this->_valid = false;
		throw std::runtime_error("invalid Method");
	}
}

void Request::parse_resource(std::string token)
{
	if (token[0] != '/')
		throw std::runtime_error("requestURI not given as absolute path");
	this->_resource = token;
}
void Request::parse_http(std::string token)
{
	if (token.substr(0, 5) != "HTTP/")
		throw std::runtime_error("invalid HTTP version");
	int pos = token.find(".");
	if (pos == std::string::npos || pos == 6 || pos == token.back()) //no . or no other char before .
		throw std::runtime_error("no HTTP version given");
	std::string smajor = token.substr(5, pos - 1);
	size_t major = stoul(smajor);
	std::string sminor = token.substr(pos + 1, token.back());
	size_t minor = stoul(sminor);
	if (major == 0 && smajor != "0" || minor == 0 && sminor != "0")
		throw std::runtime_error("no HTTP version given");
	this->_versionMajor = major;
	this->_versionMinor = minor;
	
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
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return ;
	}
	(void)bytes;
}

const bool Request::isValid() const {
	return _valid;
}

const unsigned int Request::getVersionMajor() const {
	return _versionMajor;
}

const unsigned int Request::getVersionMinor() const {
	return _versionMinor;
}

const std::string &Request::getResource() const {
	return _resource;
}

const int Request::getMethod() const {
	return _method;
}
