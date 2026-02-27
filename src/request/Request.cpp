#include "Request.hpp"
#include "Logging.hpp"
#include "NetworkingDefines.hpp"

#define MAX_REQUEST 1024
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

Request::Request() :
	ClientHungUp(false),
	_method(Generic),
	_resource(""),
	_majorVersion(0), 
	_minorVersion(0), 
	_valid(false)	
{}  

void Request::init(std::string input) {
	this->parseRequestLine(input);
}

Request::Request(std::string input)
	: _method(Generic), _resource(""), _majorVersion(0), _minorVersion(0), _valid(false)
{
	this->parseRequestLine(input);
}  
bool Request::process(int Socket)
{
    readFromSocket(Socket);  // fills _buf
	while (true)
    {
        if (_state == STATUS_LINE)
        {
			logging::log(logging::Debug, "Request::process() state is STATUS_LINE");
            if (!parseRequestLineFromBuffer())
                return false;
        }
        else if (_state == HEADERS)
        {
			logging::log(logging::Debug, "Request::process() state is HEADERS");
            if (!parseHeadersFromBuffer())
                return false;
        }
        else if (_state == COMPLETE)
		{
			logging::log(logging::Debug, "Request::process() state is COMPLETE");
    		return true;
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
	logging::log(logging::Debug, "parse status_line: split Successfull");
    return tokens;
}

void Request::readFromSocket(int Fd){
	logging::log(logging::Debug, "readFromSocket() starts");
	const ssize_t bytesRead = _buf.fileToBuf(Fd, MAX_REQUEST);
  //recv(_sock, &_readBuf, MAX_REQUEST, 0);

	if (bytesRead == MAX_REQUEST) {
    	logging::log(logging::Info, "read_data(): bytes_read == MAX REQUEST");
    // May happen frequently, will be handled in chunks
    // Logging for debug purposes as we build.
  	}
  	if (bytesRead == 0) {
    	logging::log(logging::Warning, "read_data(): bytes_read == 0");
		ClientHungUp = true; //into conditions?
    	logging::log(logging::Warning, "client appears to have hung up.");
    	return;
  	}
  	if (bytesRead < 0) {
    	std::ostringstream msg;
    	logging::log(logging::Warning, "buf.fill() not successful");
    	return;
 	}
  	logging::log(logging::Debug, "readFromSocket() done");
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

ParseState Request::getState() const{
	return _state;
}
