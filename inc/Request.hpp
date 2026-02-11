#pragma once

#include <string>

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

		const bool isValid() const;
		const unsigned int getMajorV() const;
		const unsigned int getMinorV() const;
		const std::string &getResource() const;
		const getMethod() const

	private:
		unsigned int _majorVersion;
		unsigned int _minorVersion;
		std::string _resource;
		HttpMethod _method;
		bool _valid;
};
