#include "CGIProcess.hpp"
#include "Logging.hpp"
#include "Request.hpp"
#include "Script.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SH_DEFAULT_PATH "/home/jhelbig/Desktop/webserv/scripts"
#define PY_DEFAULT_PATH "/usr/bin/python3"
#define NB_OF_ENV 8

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

static char *createEnvMember(std::string Key, std::string Value){
    logging::log3(logging::Debug, __func__, " ", Value);
	char *line = strdup((Key + "=" + Value).c_str());
    if (!line)
        return (NULL); //error handling
    return (line);
}

char **createEnv(Request Req, Script Script){
	char **env = (char **)malloc(sizeof(char *) * (NB_OF_ENV + 1));
    if (!env)
        return NULL; //error handling
	//env from serverConfig
    env[0] = createEnvMember("SERVER_NAME", Script.getServerName());
    env[1] = createEnvMember("SERVER_PORT", Script.getServerPort());
    env[2] = createEnvMember("SERVER_PROTOCOL", Script.getServerProtocol());
    env[3] = createEnvMember("SERVER_SOFTWARE", Script.getServerSoftware());
    env[4] = createEnvMember("SERVER_INTERFACE", Script.getServerInterface());
    //env from request
	env[5] = createEnvMember("REQUEST_METHOD", Req.getMethodString());
	env[6] = createEnvMember("SCRIPT_NAME", Req.getResource().substr(1));
	env[7] = createEnvMember("QUERY_STRING", Req.getQueryString());
	
    env[8] = NULL;
	return (env);
}

void CGIProcess::init(Request Req, Script Script){
	logging::log(logging::Debug, "CGI Process init called");
	// allowed methods are: Head/Get, Post
    

	// create env:
	_env = createEnv(Req, Script);
	
	//create args
	_args = (char **)malloc(sizeof(char *) * 3);
	if (!_args)
		return ;// error handling
	_args[0] = strdup("python3");
	_args[1] = strdup(Req.getResource().c_str());
	_args[2] = NULL;
	logging::log3(logging::Debug, _args[0], " ",_args[1]);
	
	//create path
		//resolve paths function - information from config file
		//for now: handling py only
	_path = strdup(PY_DEFAULT_PATH);
	
	// set up pipes
	int intoCGI[2];
	int fromCGI[2];
	if (pipe(intoCGI))
		return; //error handling
	if (pipe(fromCGI))
		return; //error handling
	logging::log(logging::Debug, "CGI init Pipes created");
	//fork
	_pid = fork(); // this pid needs to be added to poll-list
	if (_pid == -1)
		return; //error handling
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
		exit(1);
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
    return ;
}
