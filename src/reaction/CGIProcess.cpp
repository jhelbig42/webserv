#include "CGIProcess.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Request.hpp"
#include "Script.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SH_DEFAULT_PATH "/home/jhelbig/Desktop/webserv/scripts"
#define PY_DEFAULT_PATH "/usr/bin/python3"

#define EXECVE_ERR 127

CGIProcess::CGIProcess() : _env(NULL),
							_args(NULL),
							_path(NULL),
							_pid(-1),
							_forwardSocket(-1),
							_inputDone(false)

{
}

CGIProcess::~CGIProcess(){
	if (_env)
	{
    	for (int i = 0; _env[i] != NULL; ++i){
        	free(_env[i]);
    	}
    	free((void *)_env);
	}
	if (_args){
		for (int i = 0; _args[i] != NULL; ++i){
        	free(_args[i]);
    	}
    free((void *)_args);
	}
   	free(_path);
}

bool CGIProcess::isInputDone() const{
	return _inputDone;
}

int CGIProcess::getForwardSocket() const{
	return _forwardSocket;
}

int CGIProcess::getPid() const {
	return _pid;
}


void CGIProcess::setPid(pid_t pid){
	_pid = pid;
}

void CGIProcess::setInputDone(bool done){
	_inputDone = done;
}

void CGIProcess::clearEnv() {
    if (_env) {
        for (int i = 0; _env[i] != NULL; ++i) {
            if (_env[i]) 
				free(_env[i]);
        }
        free((void *)_env);
        _env = NULL;
    }
}

// Maps the Enum to the actual String Key
std::string CGIProcess::getEnvKey(EnvMembers member) const {
    static const char* keys[] = {
        "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", 
        "SERVER_SOFTWARE", "SERVER_INTERFACE", "REQUEST_METHOD", 
        "SCRIPT_NAME", "QUERY_STRING"
    };
    return keys[member];
}

// Maps the Enum to the value retrieved from Request/Script
std::string CGIProcess::getEnvValue(EnvMembers member, Request& Req, Script& Script) const {
    switch (member) {
        case SERVER_NAME:      
			return Script.getServerName();
        case SERVER_PORT:      
			return Script.getServerPort();
        case SERVER_PROTOCOL:  
			return Script.getServerProtocol();
        case SERVER_SOFTWARE:  
			return Script.getServerSoftware();
        case SERVER_INTERFACE: 
			return Script.getServerInterface();
        case REQUEST_METHOD:   
			return Req.getMethodString();
        case SCRIPT_NAME:      
			return Req.getResource().substr(1); // Remove leading '/' from Resource
        case QUERY_STRING:     
			return Req.getQueryString();
        default:               
			return "";
    }
}

bool CGIProcess::envMember(EnvMembers index, const std::string& key, const std::string& value) {
    const std::string entry = key + "=" + value;
    _env[index] = strdup(entry.c_str());
    return (_env[index] != NULL);
}

bool CGIProcess::createEnv(Request& Req, Script& Script) {
    _env = static_cast<char**>(malloc(sizeof(char*) * (NB_OF_ENV + 1)));
    if (!_env) 
		return false;

    for (int i = 0; i <= NB_OF_ENV; ++i)
		_env[i] = NULL;

    // Loop through the Enum
    for (int i = 0; i < NB_OF_ENV; ++i) {
        const EnvMembers member = static_cast<EnvMembers>(i);
        if (!envMember(member, getEnvKey(member), getEnvValue(member, Req, Script))) {
            clearEnv();
            return false;
        }
    }
    _env[NB_OF_ENV] = NULL;
    return true;
}

bool CGIProcess::createArgs(Request &Req){ 
	_args = (char **)malloc(sizeof(char *) * 3);
	if (!_args)
		return false;// error handling
	const HttpHeaders::MediaType type = Req.getHeaders().getContentType();
	switch (type){
		case HttpHeaders::ApplicationSh:
			_args[0] = strdup("bash");
			break;
		case HttpHeaders::TextPython:
			_args[0] = strdup("python3");
			break;
		default: // should never be reached
			_args[0] = NULL;
			break;
	} 
	_args[1] = strdup(Req.getResource().c_str());
	_args[2] = NULL;
	if (!_args[0] || !_args[1])
		return false;
	logging::log3(logging::Debug, _args[0], " ",_args[1]);
	return true;
}

//likely to be replaced be the general resolvePath function, that will be executed before Reaction init
bool CGIProcess::resolvePath(){
	_path = strdup(PY_DEFAULT_PATH);
	if (!_path)
		return false;
	return true;
}

//setting up the ForwardSocket, create the child process handling the script
bool CGIProcess::initForwardSocket() {
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) return false;

    _pid = fork();
    if (_pid == -1) {
        close(sv[0]); close(sv[1]);
        return false;
    }

    if (_pid == 0) { // Child
        close(sv[0]);
        dup2(sv[1], STDIN_FILENO);
        dup2(sv[1], STDOUT_FILENO);
        close(sv[1]);
        
        execve(_path, _args, _env);
        _exit(EXECVE_ERR);
    }

    // Parent
    close(sv[1]);
    _forwardSocket = sv[0];
    return true;
}


bool CGIProcess::init(Request Req, Script Script){
	logging::log(logging::Debug, "CGI Process init called");
	// allowed methods are: Head/Get, Post
	//still needs to be compared to allowed methods according to config
	// allowed methods will be handled before Reaction init, along with resolve path
	// is we are here, the method is allowed
    
	// create env:
	if (!createEnv(Req, Script))
		return false; // error handling in Reaction
						//errors here will all be 500
	//create args
	if (!createArgs(Req))
		return false;
	
	//create path
		//resolve paths function - information from config file
		//for now: handling py only
	if (!resolvePath())
		return false;
	
	if (!initForwardSocket())
		return (false);
	
    return true;
}
