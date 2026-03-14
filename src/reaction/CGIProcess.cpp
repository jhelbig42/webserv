#include "CGIProcess.hpp"
#include "Request.hpp"
#include "Script.hpp"

#include <string.h>

#define SH_DEFAULT_PATH "/bin/sh"
#define NB_OF_ENV 8

CGIProcess::CGIProcess() : env(NULL),
                            args(NULL),
                            path(NULL),
                            input_done(false)
{

}

CGIProcess::~CGIProcess(){
    for (int i = 0; env[i] != NULL; ++i){
        free(env[i]);
    }
    free(env);
    for (int i = 0; args[i] != NULL; ++i){
        free(args[i]);
    }
    free(args);
    free(path);
}

static char *createEnvMember(std::string Key, std::string Value){
    char *line = strdup((Key + "=" + Value).c_str());
    if (!line)
        return (NULL); //error handling
    return (line);
}

void CGIProcess::init(Request Req, Script Script){
    env = (char **)malloc(sizeof(char *) * (NB_OF_ENV + 1));
    if (!env)
        return ; //error handling
    env[0] = createEnvMember("SERVER_NAME", Script.getServerName());
    env[1] = createEnvMember("SERVER_PORT", Script.getServerPort());
    env[2] = createEnvMember("SERVER_PROTOCOL", Script.getServerProtocol());
    env[3] = createEnvMember("SERVER_SOFTWARE", Script.getServerSoftware());
    env[4] = createEnvMember("SERVER_INTERFACE", Script.getServerInterface());
    
    env[8] = NULL; 
    return ;
}
