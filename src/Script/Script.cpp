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

void Script::setServerName(std::string name){
    _serverName = name;
}

void Script::setServerPort(std::string port){
    _serverPort = port;
}

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