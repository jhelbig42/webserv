#pragma once

#include "Request.hpp"
#include <string>

#define STATE_BUFFSIZE 128

class PState:
{
	public:
		PState();
		PState(const PState&);
		PState& operator=(const PState&);
		~PState();

		bool init(const Request &Req);
		bool process(const Socket, const size_t bytes);

	private:
		typedef enum { In, Out, Transfer, Cgi } ProcessMode;

		bool initGet(const Request &Req);
		bool initPost(const Request &Req);
		bool initDelete(const Request &Req);

		HttpMethod _method;
		std::string _metaData;
		int _fdIn;
		int _fdOut;
		int _buffSize = STATE_BUFFSIZE;
		char _buffer[STATE_BUFFSIZE];
		int _bufStart;
		int _bufEnd;
		size_t _remainingBytes;
		ProcessMode _mode;
};
