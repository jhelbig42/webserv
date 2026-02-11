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
		const unsigned int getVersionMajor() const;
		const unsigned int getVersionMinor() const;
		const std::string &getResource() const;
		const getMethod() const

	private:
		unsigned int _versionMajor;
		unsigned int _versionMinor;
		std::string _resource;
		HttpMethod _method;
		bool _valid;
};
