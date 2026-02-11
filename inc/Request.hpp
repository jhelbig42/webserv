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
    Request(const HttpMethod Method, const std::string &Resource, const unsigned int MajorV, const unsigned int MinorV, const bool Valid);
    
    void parse(const char *, const size_t);

		const bool isValid() const;
		const unsigned int getMajorV() const;
		const unsigned int getMinorV() const;
		const std::string &getResource() const;
		const getMethod() const

	private:
		HttpMethod _method;
		std::string _resource;
		unsigned int _majorVersion;
		unsigned int _minorVersion;
		bool _valid;
};
