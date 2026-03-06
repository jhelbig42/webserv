#include "Conditions.hpp"
#include "Logging.hpp"
#include "Request.hpp"
#include <string>
#include <vector>

#define MAX_REQUEST 1024


Request::Request() :
	_method(Generic),
	_resource(""),
	_majorVersion(0), 
	_minorVersion(0), 
	_conditions (SockRead),
	_state(STATUS_LINE)
{
	_headers.unsetAll();
}  

void Request::init(const std::string &input) {
	this->parseRequestLine(input);
}

void Request::reset() {
	_method = Generic;
	_resource = "";
	_majorVersion = 0; 
	_minorVersion = 0;
	_state = STATUS_LINE;
	_conditions = SockRead;
	_headers.unsetAll();
}


Request::Request(std::string &input)
	: _method(Generic), _resource(""), _majorVersion(0), _minorVersion(0)
{
	this->parseRequestLine(input);
}  

void Request::process(int Socket)
{
    readFromSocket(Socket);  // fills _buf
	while (true)
    {
		if (_state == CLIENTHUNGUP)
			return ;
        if (_state == STATUS_LINE)
        {
			logging::log(logging::Debug, "Request::process() state is STATUS_LINE");
            if (!parseRequestLineFromBuffer())
                return ;
        }
        else if (_state == HEADERS)
        {
			logging::log(logging::Debug, "Request::process() state is HEADERS");
            if (!parseHeadersFromBuffer())
                return ;
        }
        else if (_state == COMPLETE)
		{
			logging::log(logging::Debug, "Request::process() state is COMPLETE");
    		return ;
		}
		else if (_state == INVALID)
		{
			logging::log(logging::Debug, "Request::process() state is INVALID");
    		return ;
		}
    }
}

std::vector<std::string> split(const std::string& S, const std::string& Delimiter) 
{
    std::vector<std::string> tokens;
    std::string token;
	size_t last = 0;
	size_t next = 0;
    while ((next = S.find(Delimiter, last)) != std::string::npos) {
        token = S.substr(last, next - last);
        tokens.push_back(token);
		last = next + Delimiter.length();
    }
    tokens.push_back(S.substr(last)); 
	logging::log(logging::Debug, "split Successfull");
    return tokens;
}

void Request::readFromSocket(int Fd){
	logging::log(logging::Debug, "readFromSocket() starts");
	const ssize_t bytesRead = _buf.fileToBuf(Fd, MAX_REQUEST);

	if (bytesRead == MAX_REQUEST) {
    	logging::log(logging::Info, "read_data(): bytes_read == MAX REQUEST");
    // May happen frequently, will be handled in chunks
    // Logging for debug purposes as we build.
  	}
  	if (bytesRead == 0) {
    	logging::log(logging::Warning, "read_data(): bytes_read == 0");
		_state = CLIENTHUNGUP;
    	logging::log(logging::Warning, "client appears to have hung up.");
    	return;
  	}
  	if (bytesRead < 0) {
    	logging::log(logging::Warning, "buf.fill() not successful");
    	return;
 	}
  	logging::log(logging::Debug, "readFromSocket() done");
}

Buffer		Request::getBuffer() const{
	return _buf;
}

HttpHeaders	Request::getHeaders() const{
	return _headers;
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

Conditions Request::getConditions(void) const {
	return _conditions;
}

ParseState Request::getState() const{
	return _state;
}

