#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

typedef enum {
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
    
    	void parse(const char *, const size_t);
		void parse_method(std::string token);
		void parse_resource(std::string token);
		void parse_http(std::string token);

		const bool isValid() const;
		const unsigned int getVersionMajor() const;
		const unsigned int getVersionMinor() const;
		const std::string &getResource() const;
		const int getMethod() const;

	private:
		unsigned int _versionMajor;
		unsigned int _versionMinor;
		std::string _resource;
		HttpMethod _method;
		bool _valid;
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
