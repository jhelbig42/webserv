#pragma once

#include "Conditions.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include "Logging.hpp"

typedef enum {
  Generic,
	Get,
	Post,
	Delete
} HttpMethod;

class Request {
	public:
		Request();
		// request(const request&);
		// request& operator=(const request&);
		// ~request();
    	// Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid);
		explicit Request(const char *input);
		void init(const char *input);
    	
		void parseStatusLine(const char *, const size_t);
		void parseMethod(std::string token);
		void parseResource(std::string token);
		void parseHttp(std::string token);

		bool isFullyParsed(void) const;
		Conditions getConditions(void) const;
		bool process(const int Socket, const size_t Bytes);

		bool isValid() const;
		size_t getMajorV() const;
		size_t getMinorV() const;
		const std::string &getResource() const;
		HttpMethod getMethod() const;

	private:
		HttpMethod _method;
		std::string _resource;
		size_t _majorVersion;
		size_t _minorVersion;
		bool _valid;
		Conditions _conditions;
		bool _fullyParsed;
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
