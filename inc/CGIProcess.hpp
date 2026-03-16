#pragma once

#include "Request.hpp"
#include "Script.hpp"
#include <string>

class CGIProcess
{
public:
    CGIProcess();
    ~CGIProcess();

    void init(Request Req, Script Script);

	bool isInputDone() const;
	int getReadFd() const;

private:
    char **_env;
    char **_args;
    char *_path;
    
    pid_t _pid;

    int _writeIntoCGI;   // write request body to CGI
    int _readFromCGI;  // read CGI output

    std::string output;
    bool _inputDone;
};
