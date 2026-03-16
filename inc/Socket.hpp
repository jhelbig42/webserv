#pragma once

#include "Website.hpp"

class Socket {

	private:
	int _fd;
	bool _listening;
	const Website &website;

	public:
	Socket(const int Fd, const bool Listening, const Website &web);
	int getFd();
	bool isListening();
	const Website &getWebsite();
};
