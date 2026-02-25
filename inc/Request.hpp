#pragma once

#include "Buffer.hpp"
#include "Conditions.hpp"
#include "Logging.hpp"
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
    	
		void readFromSocket(int Fd);
		bool ClientHungUp;
		bool parseStatusLineFromBuffer();
		void parseStatusLine(std::string input);
		bool parseHeadersFromBuffer();
		void parseHeader(std::string headerLine);
		void parseMethod(std::string token);
		void parseResource(std::string token);
		void parseHttp(std::string token);

		bool isFullyParsed(void) const;
		Conditions getConditions(void) const;
		bool process(const int Socket);

		ParseState	getState() const;
		bool		isValid() const;
		size_t		getMajorV() const;
		size_t		getMinorV() const;
		const std::string &getResource() const;
		HttpMethod 	getMethod() const;

	private:
		HttpMethod	_method;
		std::string _resource;
		size_t		_majorVersion;
		size_t		_minorVersion;
		bool		_valid;
		Conditions	_conditions;
		//bool		_statusLineParsed;
		bool		_fullyParsed;
		Buffer		_buf;
		ParseState	_state;
		
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
