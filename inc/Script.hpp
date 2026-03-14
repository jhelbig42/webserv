#pragma once

#include <string>

class Script{
public:
    Script( void );
    ~Script( void );

    //do these function directly return the argv for env?
    //would need to return cstrings
    std::string getServerName();
    std::string getServerPort();
    std::string getServerProtocol();
    std::string getServerSoftware();
    std::string getServerInterface();

    void setServerName(std::string name);
    void setServerPort(std::string port);

protected:
    //from Server configs
    std::string _serverName;
    std::string _serverPort;
    const std::string _serverProtocol;
    const std::string _serverSoftware;
    const std::string _serverInterface;

    //from Request
    std::string _requestMethod;
    std::string _scriptName;
    std::string _queryString;

};

