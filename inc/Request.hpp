#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include "Logging.hpp"

typedef enum {
  Generic,
	Get,
	Post,
	Delete
} HttpMethod;

class Request {
	public:
		// request();
		// request(const request&);
		// request& operator=(const request&);
		// ~request();
    	Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid);
		Request(const char *input);
    	
		void parse(const char *, const size_t);
		void parse_method(std::string token);
		void parse_resource(std::string token);
		void parse_http(std::string token);

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
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
