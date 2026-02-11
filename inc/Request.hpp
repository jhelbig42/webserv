#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

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
    
    	void parse(const char *, const size_t);
		void parse_method(std::string token);
		void parse_resource(std::string token);
		void parse_http(std::string token);

		bool isValid() const;
		unsigned int getMajorV() const;
		unsigned int getMinorV() const;
		const std::string &getResource() const;
		HttpMethod getMethod() const;
		
	private:
		HttpMethod _method;
		std::string _resource;
		unsigned int _majorVersion;
		unsigned int _minorVersion;
		bool _valid;
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
