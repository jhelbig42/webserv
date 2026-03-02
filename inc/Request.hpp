#pragma once

#include "Buffer.hpp"
#include "Conditions.hpp"
#include "Logging.hpp"
#include "HttpHeaders.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

typedef enum {
  Generic,
	Head,
	Get,
	Post,
	Delete
} HttpMethod;

typedef enum  {
    STATUS_LINE,
    HEADERS,
    COMPLETE,
	INVALID
} ParseState;


class Request {
	public:


/// called on initialisation of Connection.
		Request();
		// request(const request&);
		// request& operator=(const request&);
		// ~request();
    	// Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid);
		
		explicit Request(std::string &input); //just used in test main
		void init(const std::string &input); // to be removed

/// \brief Resets to default values to be ready for new Request
/// Just neccessarry if Connection is kept alive
		void reset( void );
    	
		bool process(const int Socket);
		void readFromSocket(int Fd); // this function s
		
		bool ClientHungUp; // will go into state
		
		bool parseRequestLineFromBuffer();
		void parseRequestLine(const std::string &RequestLine);
		void parseMethod(const std::string &Token);
		void parseResource(const std::string &Token);
		void parseHttp(const std::string &Token);

		bool parseHeadersFromBuffer();
		void parseHeader(const std::string &headerLine);
		
		Conditions getConditions(void) const;

		ParseState	getState() const;
		//bool		isValid() const;
		size_t		getMajorV() const;
		size_t		getMinorV() const;
		const std::string &getResource() const;
		HttpMethod 	getMethod() const;
		bool		isFullyParsed() const;

	private:
		HttpMethod	_method;
		std::string _resource;
		size_t		_majorVersion;
		size_t		_minorVersion;
		Conditions	_conditions;
		bool		_fullyParsed; //likely obsolete as caught by _state --> TO DO delete
		Buffer		_buf;
		ParseState	_state;
		HttpHeaders	_headers;	
		//bool		_valid; // can this go into state as well?	
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
