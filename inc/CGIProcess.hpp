#pragma once

#include "Request.hpp"
#include "Script.hpp"
#include <string>

class CGIProcess {
public:
    enum EnvMembers {
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

	/// @fn bool init(Request Req, Script Script)
    /// @brief initialize Reaction if Request calls for CGI execution
    /// @param Req 
    /// @param Script 
    /// @return false on any error
	/// This includes creating a child process using fork(). This child process
	// will execute the CGI script. Within the function the environment 
	/// variables and arguments that are needed for the scripts execution are 
	/// set.
	/// The filedescriptor leading of the created socket is saved a member of 
	/// the CGIProcess Class. The coresponding instance of Connection will 
	/// check for it an include it in the poll() map.
    bool init(Request Req, Script Script, const std::string &Path);
    bool createEnv(Request& Req, Script& Script);
    bool createArgs(Request &Req, const std::string &Path);
	bool initForwardSocket();
    
    //Getters
    bool isInputDone() const;
	int getPid() const ;
    int getForwardSocket() const;

    //setters
    void setPid(pid_t Pid);
    void setInputDone(bool Done);
	void setCGIPath(std::string const &Path);
    
private:
   
    bool envMember(EnvMembers Index, const std::string& Key, const std::string& Value);
    
	void clearEnv();

    std::string getEnvKey(EnvMembers Member) const;
    std::string getEnvValue(EnvMembers Member, Request& Req, Script& Script) const;

    char**      _env;
    char**      _args;
    char*       _path;
    pid_t       _pid;
    int         _forwardSocket;
    std::string _output;
    bool        _inputDone;
};
