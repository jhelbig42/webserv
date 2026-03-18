#include "CGIProcess.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Request.hpp"
#include "Reaction.hpp"
#include "Script.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SH_DEFAULT_PATH "/home/jhelbig/Desktop/webserv/scripts"
#define PY_DEFAULT_PATH "/usr/bin/python3"

CGIProcess::CGIProcess() : _env(NULL),
							_args(NULL),
							_path(NULL),
                            _inputDone(false)

{
}

CGIProcess::~CGIProcess(){
	if (_env)
	{
    	for (int i = 0; _env[i] != NULL; ++i){
        	free(_env[i]);
    	}
    	free(_env);
	}
	if (_args){
		for (int i = 0; _args[i] != NULL; ++i){
        	free(_args[i]);
    	}
    free(_args);
	}
   	free(_path);
}

bool CGIProcess::isInputDone() const{
	return _inputDone;
}

int CGIProcess::getReadFd() const
{
    return _readFromCGI;
}

int CGIProcess::getWriteFd() const
{
    return _writeIntoCGI;
}

void CGIProcess::_clearEnv() {
    if (_env) {
        for (int i = 0; i < NB_OF_ENV; ++i) {
            if (_env[i]) free(_env[i]);
        }
        free(_env);
        _env = NULL;
    }
}

// Maps the Enum to the actual String Key
std::string CGIProcess::_getEnvKey(envMembers member) const {
    static const char* keys[] = {
        "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", 
        "SERVER_SOFTWARE", "SERVER_INTERFACE", "REQUEST_METHOD", 
        "SCRIPT_NAME", "QUERY_STRING"
    };
    return keys[member];
}

// Maps the Enum to the value retrieved from Request/Script
std::string CGIProcess::_getEnvValue(envMembers member, Request& Req, Script& Script) const {
    switch (member) {
        case SERVER_NAME:      return Script.getServerName();
        case SERVER_PORT:      return Script.getServerPort();
        case SERVER_PROTOCOL:  return Script.getServerProtocol();
        case SERVER_SOFTWARE:  return Script.getServerSoftware();
        case SERVER_INTERFACE: return Script.getServerInterface();
        case REQUEST_METHOD:   return Req.getMethodString();
        case SCRIPT_NAME:      return Req.getResource().substr(1); // Remove leading '/'
        case QUERY_STRING:     return Req.getQueryString();
        default:               return "";
    }
}

bool CGIProcess::_envMember(envMembers index, const std::string& key, const std::string& value) {
    std::string entry = key + "=" + value;
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
        envMembers member = static_cast<envMembers>(i);
        if (!_envMember(member, _getEnvKey(member), _getEnvValue(member, Req, Script))) {
            _clearEnv();
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
	HttpHeaders::MediaType type = Req.getHeaders().getContentType();
	switch (type){
		case HttpHeaders::ApplicationSh:
			_args[0] = strdup("bash");
			break;
		case HttpHeaders::TextPython:
			_args[0] = strdup("python3");
			break;
		default: // should never be reached
			break;
	} 
	_args[1] = strdup(Req.getResource().c_str());
	_args[2] = NULL;
	if (!_args[0] || !_args[1])
		return false;
	logging::log3(logging::Debug, _args[0], " ",_args[1]);
	return true;
}

bool CGIProcess::resolvePath(){
	_path = strdup(PY_DEFAULT_PATH);
	if (!_path)
		return false;
	return true;
}

bool CGIProcess::init(Request Req, Script Script){
	logging::log(logging::Debug, "CGI Process init called");
	// allowed methods are: Head/Get, Post
	//still needs to be compared to allowed methods according to config
    
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
	
	// set up pipes
	int intoCGI[2];
	int fromCGI[2];
	if (pipe(intoCGI))
		return false; //error handling
	if (pipe(fromCGI))
		return false; //error handling
	logging::log(logging::Debug, "CGI init Pipes created");
	//fork
	_pid = fork(); // this pid needs to be added to poll-list
	if (_pid == -1)
		return false; //error handling
	if (_pid == 0)
	{
		//we are in child
		close(intoCGI[1]); //write side
		close(fromCGI[0]); //read side
		dup2(intoCGI[0], STDIN_FILENO);
		dup2(fromCGI[1], STDOUT_FILENO);
		close(intoCGI[0]);
		close(fromCGI[1]);
		execve(_path, _args, _env);
		logging::log(logging::Debug, "execve failed in child");
		_exit(1);
	}
	//we are in parent here
	logging::log(logging::Debug, "CGI init I am the parent");
	
	close(intoCGI[0]);
	close(fromCGI[1]);
	waitpid(_pid, 0, 0);
	_writeIntoCGI = intoCGI[1];
	_readFromCGI = fromCGI[0];

	//needs to go! just for testing for the moment
	//will be true for every CGI get Request 
	//jsut post requests have a body
	_inputDone = true;
    return true;
}
