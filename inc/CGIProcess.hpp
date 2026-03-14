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

private:
    char **env;
    char **args;
    char *path;
    
    pid_t pid;

    int writeIntoCGI;   // write request body to CGI
    int readFromCGI;  // read CGI output

    std::string output;
    bool input_done;
};