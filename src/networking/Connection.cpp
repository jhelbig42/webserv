#include "Connection.hpp"

Connection::Connection(void): _index(-42), _active(false), _sock(-42), _addr_size(0){
	memset(&_info, 0, sizeof _info);
	memset(&_request_buffer, '\0', sizeof _request_buffer);
}

Connection::~Connection(void){}

void Connection::clear_connection(){
	_active = false;
	_sock = -42;
	_addr_size = 0;
	memset(&_info, 0, sizeof _info);
	memset(&_request_buffer, '\0', sizeof _request_buffer);
}
