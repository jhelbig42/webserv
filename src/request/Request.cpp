#include "Request.hpp"
#include <stdexcept>
#include "HttpMethods.hpp"
#include "Logging.hpp"
#include <string>
#include <sys/types.h>
#include <vector>

#define MAX_REQUEST 1024

Request::Request()
    : _method(Generic), _resource(""), _majorVersion(0), _minorVersion(0),
      _state(STATUS_LINE), _queryString("") {
  _headers.unsetAll();
}

void Request::init(const std::string &Input) {
  this->parseRequestLine(Input);
}

void Request::reset() {
  _method = Generic;
  _resource = "";
  _majorVersion = 0;
  _minorVersion = 0;
  _state = STATUS_LINE;
  _headers.unsetAll();
}

void Request::process(int Socket) {
  readFromSocket(Socket); // fills _buf
  while (true) {
    if (_state == CLIENTHUNGUP)
      return;
    if (_state == STATUS_LINE) {
      logging::log(logging::Debug, "Request::process() state is STATUS_LINE");
      if (!parseRequestLineFromBuffer())
        return;
    } else if (_state == HEADERS) {
      logging::log(logging::Debug, "Request::process() state is HEADERS");
      if (!parseHeadersFromBuffer())
        return;
    } else if (_state == COMPLETE) {
      logging::log(logging::Debug, "Request::process() state is COMPLETE");
      return;
    } else if (_state == INVALID) {
      logging::log(logging::Debug, "Request::process() state is INVALID");
      return;
    }
  }
}

std::vector<std::string> split(const std::string &S) {
  std::vector<std::string> tokens;
  std::stringstream iss(S);
  std::string token;

  while ((iss >> token)) {
    tokens.push_back(token);
  }
  logging::log2(logging::Debug, "size of tokens is ", tokens.size());
  logging::log2(logging::Debug, "last token", tokens[tokens.size() -1]);
  return tokens;
}

void Request::readFromSocket(int Fd) {
  logging::log(logging::Debug, "readFromSocket() starts");
  //const ssize_t bytesRead = _buf.fileToBuf(Fd, MAX_REQUEST);
	ssize_t bytesRead;
	try {
		bytesRead = _buf.fileToBuf(Fd, MAX_REQUEST);
	} catch (std::runtime_error &e) {
		// client disconnected before/while we tried to read
		logging::log3(logging::Info, "readFromSocket: read from client failed (",
		              e.what(), "), treating as hangup");
		_state = CLIENTHUNGUP;
		return;
	}
  if (bytesRead == MAX_REQUEST) {
    logging::log(logging::Info, "read_data(): bytes_read == MAX REQUEST");
    // May happen frequently, will be handled in chunks
    // Logging for debug purposes as we build.
  }
  if (bytesRead == 0) {
    logging::log(logging::Info, "read_data(): bytes_read == 0");
    _state = CLIENTHUNGUP;
    logging::log(logging::Info, "client appears to have hung up.");
    return;
  }
  if (bytesRead < 0) {
    logging::log(logging::Warning, "buf.fill() not successful");
    //	logging::log2(logging::Warning, "bytesRead = ", bytesRead);
    return;
  }
  logging::log(logging::Debug, "readFromSocket() done");
}

Buffer Request::getBuffer() const {
  return _buf;
}

HttpHeaders Request::getHeaders() const {
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

std::string Request::getMethodString() const {
  switch (_method) {
  case Head:
    return "HEAD";
    break;
  case Get:
    return "GET";
    break;
  case Post:
    return "POST";
    break;
  case Delete:
    return "DELETE";
    break;
  default:
    return "INVALID";
  }
}

ParseState Request::getState() const {
  return _state;
}

std::string Request::getQueryString(void) const {
  return _queryString;
}
