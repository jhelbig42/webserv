#pragma once

#include "Request.hpp"
#include "Script.hpp"
#include <string>

class CGIProcess {
public:
    enum envMembers {
        SERVER_NAME,
        SERVER_PORT,
        SERVER_PROTOCOL,
        SERVER_SOFTWARE,
        SERVER_INTERFACE,
        REQUEST_METHOD,
        SCRIPT_NAME,
        QUERY_STRING,
        NB_OF_ENV 
    };

    CGIProcess();
    ~CGIProcess();

    bool init(Request Req, Script Script);
    bool createEnv(Request& Req, Script& Script);
    bool createArgs(Request &Req);
    bool resolvePath();
	bool initPipes();
    
    //Getters
    bool isInputDone() const;
	int getPid() const ;
    int getForwardSocket() const;

    //setters
    void setPid(pid_t pid);
    void setInputDone(bool done);
    
private:
    void _clearEnv();
    bool _envMember(envMembers index, const std::string& key, const std::string& value);
    
    std::string _getEnvKey(envMembers member) const;
    std::string _getEnvValue(envMembers member, Request& Req, Script& Script) const;

    char**      _env;
    char**      _args;
    char*       _path;
    pid_t       _pid;
    int         _forwardSocket;
    std::string _output;
    bool        _inputDone;
};
