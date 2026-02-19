#pragma once

#include "Buffer.hpp"
#include "HttpHeaders.hpp"
#include "Request.hpp"
#include <string>
#include <sys/types.h>

class Response {
public:
  Response();
  // Response(const Response&);
  // Response& operator=(const Response&);
  // ~Response();

  explicit Response(const Request &Req);
  void init(const Request &Req);

  /// \fn bool process(const int Socket, const size_t Bytes);
  /// \brief continues processing a response object
  ///
  /// Although Bytes gives a hint on how many Bytes should be processed,
  /// this is not a guarantee.
  /// process() will decide by itself what is most convenient.
  ///
  /// This means:
  /// Usually only one system call or other heavy operation will be performed.
  /// When any read() or write() call happens,
  /// usually no more than Bytes bytes will be processed by such call.
  ///
  /// \param Socket socket associated with response
  /// \param Bytes the maximum amount of Bytes to process by system calls
  ///
  /// \return true if response got fully processed otherwise false
  bool process(const int Socket, const size_t Bytes);

private:
  typedef enum { None, SendFile, ReceiveFile, Cgi } ProcessType;

  // sending files + metadata
  bool sendFile(const int Socket, const size_t Bytes);
  void initSendFile(const int Code, const char *File);
  bool statbufPopulate(const int Code, const char *File, struct stat &statbuf);
  bool setFdIn(const char *File);
  bool initError(const int Errno);

  HttpHeaders _headers;

  ProcessType _ptype;

  // consider abstraction for metaData
  bool _metadataSent;
  std::string _metadata;

  int _fdIn;
  int _fdOut;

  // consider abstraction for buffer
  Buffer _buffer;
};
