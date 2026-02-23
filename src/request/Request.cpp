#include "Request.hpp"
#include "Logging.hpp"

// Source - https://stackoverflow.com/a/14266139
// Posted by Vincenzo Pii, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-11, License - CC BY-SA 4.0

// Request::Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid)
//   : _method(Method), _resource(Resource), _majorVersion(MajorV), _minorVersion(MinorV), _valid(Valid) {}

/**
 * \brief Request Constructor called on given input.
 * This should be used if the status line. Later: called on first buffer.
 * Calls parse() that parses the statusline.
 * Does not need the amount of sent bytes yet, but will likely do so,
 * when the headers are sent and need to be parsed later on.
 * 
 * \return fills the Request attributes. On error _valid attribute will remain false
 */

Request::Request() : _method(Generic), _resource(""), _majorVersion(0), _minorVersion(0), _valid(false)
{}  

void Request::init(const char *input) {
	this->parseStatusLine(input, 100);
}

Request::Request(const char *input)
	: _method(Generic), _resource(""), _majorVersion(0), _minorVersion(0), _valid(false)
{
	this->parseStatusLine(input, 100);
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
    tokens.push_back(s.substr(last)); 
	logging::log(logging::Debug, "parse status_line: split Successfull");
    return tokens;
}

void Request::parseMethod(std::string token)
{
	if (token == "GET")
		this->_method = Get;
	else if (token == "POST")
		this->_method = Post;
	else if (token == "DELETE")
		this->_method = Delete;
	else
		throw std::runtime_error("invalid Method");
	logging::log(logging::Debug, "parse status_line: got method successfully");
}

void Request::parseResource(std::string token)
{
	if (token[0] != '/')
		throw std::runtime_error("requestURI not given as absolute path");
	this->_resource = token;
	logging::log(logging::Debug, "parse status_line: parse resource successfully");
}
/**
 * \brief Parses the HTTP-version and sets this Version as Request attributes.
 * parseHttp uses stringstream into size_t as this gives the option to catch
 * invalid inputs like 1.1a(by setting the failbit) which is more diffiult with atoi(which would just return 0 on error, where further checks are necessary)
 * \return nothing but fills the Request attributes _majorVersion and _minorVersion. On error exceptions are thrown, that will be caught in parseStatusLine()
 */
void Request::parseHttp(std::string token)
{
	if (token.substr(0, 5) != "HTTP/")
		throw std::runtime_error("invalid HTTP version");
	size_t tback = token.size();
	size_t pos = token.find(".");
	if (pos == std::string::npos || pos <= 5 || pos == tback - 1 ) //no . or no other char before .
		throw std::runtime_error("no HTTP version given");
	
	std::stringstream smajor(token.substr(5, pos - 5));
	if (!(smajor >> this->_majorVersion) || !smajor.eof())
    	throw std::runtime_error("no HTTP version given");

	std::stringstream sminor(token.substr(pos + 1));
	if (!(sminor >> this->_minorVersion) || !sminor.eof())
    	throw std::runtime_error("no HTTP version given");

	logging::log(logging::Debug, "parse status_line: parse http version successfully");
}
/**
 * \brief Parses the status line to construct a Request instance. 
 * On any invalid syntax within the status line Request remains invalid. 
 * So before generating a Response, it shall be checked if the Request is valid at all. 
 * 
 * \returns nothing. On success Request instance will be initialized with _valid set to true. On error _valid will remain false.
 * 
 * \param buffer handed over from connection. does not yet need bytes, as it is assumed currently to only get the full statusline and nothing else
 */
void Request::parseStatusLine(const char *buffer, const size_t bytes)
{
	std::vector<std::string> status = split(buffer, " ");
	if (status.size() != 3)
	{
		this->_valid =false;
		return ;
	}
	try
	{
		parseMethod(status[0]);
		parseResource(status[1]);
		parseHttp(status[2]);
		this->_valid = true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
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

bool Request::isFullyParsed(void) const {
	return _fullyParsed;
}

Conditions Request::getConditions(void) const {
	return _conditions;
}

bool Request::process(const int Socket, const size_t Bytes) {
	return false;
}
