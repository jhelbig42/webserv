#pragma once

#include "Buffer.hpp"
#include "CGIProcess.hpp"
#include "HttpHeaders.hpp"
#include "Request.hpp"
#include "Script.hpp"
#include <string>
#include <sys/types.h>

class Reaction {
public:

	typedef enum { 
		NotInitialized,
		SendFile,
		ReceiveFile, 
		Cgi } ProcessType;
	
	Reaction();
    explicit Reaction(const Request &Req);
	
	/// @fn void init(const Request &Req)
    /// @brief initialize Reaction from Request
    /// @param Req
	/// The function processes the Request object. During this inavlid Requests
	/// cause a Error File to be send.
	/// Function also determines if Request is pointing to a CGI or not. It is
	/// decided then will the specific function need to initialize a Response 
	/// depending on the Request Method.
	///

	void init(const Request &Req);

  /// \fn bool process(const int Socket, const size_t Bytes, const int Condition);
  /// \brief continues processing a Reaction object
  ///
  /// Depending on the given Conditions, process() decides how to continue
  /// processing the Reaction object. This funciton is also open to the 
  /// possibilty that several Conditions can be true withinthis Connection.
  /// It is for instance possible that the Reaction receives from the CGI
  /// socket and sends to the client within one call.
  /// Although Bytes gives a hint on how many Bytes should be processed,
  /// this is not a guarantee.
  /// process() will decide by itself what is most convenient.
  ///
  /// This means:
  /// Usually only one system call or other heavy operation will be performed.
  /// When any read() or write() call happens,
  /// usually no more than Bytes bytes will be processed by such call.
  ///
  /// If this Reaction is running a CGI, process() it will also call
  /// checkOnChild() to ensure the child did not exit with an error.
  /// 
  /// \param Socket socket associated with Reaction
  /// \param Bytes the maximum amount of Bytes to process by system calls
  /// \param Condition the Conditions that are fullfilled for the corresponding
  /// Conncetion within this specific Call
  ///
  /// \return true if Reaction got fully processed otherwise false
  bool process(const int Socket, const size_t Bytes, const int Condition);

  Conditions	getConditions(void) const;
  ProcessType	getProcessType(void) const;
  int			getForwardSocket(void) const;

private:
  // sending files + metadata
  bool sendFile(const int Socket, const size_t Bytes);
  void initSendFile(const int Code, const char *File);
  bool statbufPopulate(const int Code, const char *File, struct stat &StatBuf);
  bool setFdIn(const int Code, const char *File);
  bool initError(const int Errno);
  void setDefaults(void);

  void initMethodNonCGI(const Request &Req);
  void initHeadGet(const Request &Req);
  void initDelete(const Request &Req);
  void initPost(const Request &Req);
  
  void initCGIMethod(const Request &Req);

  // used to handle POST Requests
  bool	receiveFile(const int Socket, const size_t Bytes);

  // called by process()
  /// \fn checkOnChild(void)
  /// \brief checks if the child process belonging to a CGI is finished.
  /// \return true if there is no CGI, or if the CGI exited without error.
  /// 		Returns false on any error otherwise.
  bool checkOnChild(void);
  void receiveFromCGI(const size_t Bytes);
  void sendToCGI(const size_t Bytes);
  void recvFromClient(const int Socket, const size_t Bytes);
  bool sendToClient(const int Socket, const size_t Bytes);

  //called within init()
  bool isCGI(const Request &Req);

  Conditions  _conditions;
  HttpHeaders _headers;
  ProcessType _processType;
  size_t      _reqContLen;
  size_t      _receivedContLen;

  // consider abstraction for metaData
  bool			_metadataSent;
  std::string 	_metadata;

  int     		_fdIn;
  FILE    		*_fdOut;
  Buffer  		_buffer;

  Script  		_script;
  CGIProcess 	_cgi; 
};
