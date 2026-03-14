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

protected:
    //from Server configs
    std::string _serverName;
    std::string _serverPort;
    std::string _serverProtocol;
    std::string _serverSoftware;
    std::string _serverInterface;   

};
