#pragma once

#include <string>

class Script{
public:
    Script( void );
    ~Script( void );

    std::string getServerName();
    std::string getServerPort();
    std::string getServerProtocol();
    std::string getServerSoftware();
    std::string getServerInterface();
    std::string getRequestMethod();
    std::string getScriptName();
    std::string getQueryString();

    void setServerName(std::string &Name);
    void setServerPort(std::string &Port);
    void setRequestMethod(std::string &Method);
    void setScriptName(std::string &ScriptName);

private:
    //from Server configs
    std::string _serverName;
    std::string _serverPort;
    const std::string _serverProtocol;
    const std::string _serverSoftware;
    const std::string _serverInterface;

    //from Request
    std::string _requestMethod;
    std::string _scriptName;
};

