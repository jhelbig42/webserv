#pragma once

#include "Buffer.hpp"
#include "Conditions.hpp"
#include "HttpHeaders.hpp"
#include "HttpMethods.hpp"
#include "Logging.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

typedef enum {
  STATUS_LINE,
  HEADERS,
  COMPLETE,
  INVALID,
  CLIENTHUNGUP
} ParseState;

class Request {
	public:
		Request();
		
		explicit Request(std::string &Input); //just used in test main
		void init(const std::string &Input); // to be removed

	/// \brief Resets to default values to be ready for new Request
	/// Just neccessarry if Connection is kept alive
		void reset( void );

	/// \brief process() handles reading from socket into Request and parsing until the Request is complete
	/// A Request is considered complete, when the StatusLine and Headers are read.
	/// This is the case when "\r\n\r\n" is found.
		void process(const int Socket);
		void readFromSocket(int Fd);
		
	///\brief Parses the status line to construct a Request instance. 
	///On any invalid syntax within the status line Request is set to be INVALID. 
	///Otherwise, when a 
		bool parseRequestLineFromBuffer();
		void parseRequestLine(const std::string &RequestLine);
		void parseMethod(const std::string &Token);
		void parseResource(const std::string &Token);
	
 	
	/// \brief Parses the HTTP-version and sets this an attribute of the 
	/// Request instance.
 	/// parseHttp uses stringstream into size_t as this gives the option
	/// to catch invalid inputs like 1.1a(by setting the failbit) which is
	/// more diffiult with atoi(which would just return 0 on error, where
	/// further checks are necessary)
		void parseHttp(const std::string &Token);

		bool parseHeadersFromBuffer();
		void parseHeader(const std::string &HeaderLine);
		
	
		Buffer		getBuffer() const;
		ParseState	getState() const;
		size_t		getMajorV() const;
		size_t		getMinorV() const;
		const std::string &getResource() const;
		HttpMethod 	getMethod() const;
		std::string getMethodString() const;
		HttpHeaders	getHeaders() const;
		std::string getQueryString(void) const;
		void setRequestContentType(void);

	private:
		HttpMethod	_method;
		std::string _resource;
		size_t		_majorVersion;
		size_t		_minorVersion;
		Buffer		_buf;
		ParseState	_state;
		HttpHeaders	_headers;
		std::string	_queryString;	
};

std::vector<std::string> split(const std::string &S,
                               const std::string &Delimiter);
