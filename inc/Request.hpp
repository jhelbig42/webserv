#pragma once

#include "Buffer.hpp"
#include "Conditions.hpp"
#include "Logging.hpp"
#include "HttpHeaders.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <sstream>

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
    COMPLETE
} ParseState;


class Request {
	public:
		Request();
		// request(const request&);
		// request& operator=(const request&);
		// ~request();
    	// Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid);
		explicit Request(std::string input);
		void init(std::string input);
    	
		bool process(const int Socket);
		void readFromSocket(int Fd);
		bool ClientHungUp;
		
		bool parseRequestLineFromBuffer();
		void parseRequestLine(std::string StatusLine);
		void parseMethod(std::string Token);
		void parseResource(std::string Token);
		void parseHttp(std::string Token);


		bool parseHeadersFromBuffer();
		void parseHeader(std::string headerLine);
		
		Conditions getConditions(void) const;

		ParseState	getState() const;
		bool		isValid() const;
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
		bool		_valid; // can this go into state as well?
		Conditions	_conditions;
		bool		_fullyParsed; //likely obsolete as caught by _state --> TO DO delete
		Buffer		_buf;
		ParseState	_state;
		HttpHeaders	_headers;
		
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
