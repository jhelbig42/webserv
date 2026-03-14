#include "Script.hpp"

#include <string>

Script::Script() : _serverProtocol("HTTP/1.0"),
                    _serverSoftware("webserv/1.0"),
                    _serverInterface("CGI/1.1")
{
    //get the information from the serverconfig File here
    setServerName("name");
    setServerPort("1234");
}

Script::~Script()
{
    
}

void Script::setServerName(std::string ServerName){
    _serverName = ServerName;
}

void Script::setServerPort(std::string Port){
    _serverPort = Port;
}

void Script::setRequestMethod(std::string Method){
    _requestMethod = Method;
}

void Script::setScriptName(std::string ScriptName){
    _serverPort = ScriptName;
}

//void Script::setQueryString(std::string QueryString){
//    _serverPort = QueryString;
//    //read from Requestbody until finished
//}

std::string Script::getServerName(){
    return _serverName;
}

std::string Script::getServerPort(){
    return _serverPort;
}

std::string Script::getServerProtocol(){
    return _serverProtocol;
}

std::string Script::getServerSoftware(){
    return _serverSoftware;
}

std::string Script::getServerInterface(){
    return _serverInterface;
}

std::string Script::getRequestMethod(){
    return _requestMethod;
}

std::string Script::getScriptName(){
    return _scriptName;
}

//std::string Script::getQueryString(){
//    return _queryString;
//}