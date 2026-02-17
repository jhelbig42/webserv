#pragma once

#include "Buffer.hpp"
#include "Request.hpp"
#include <string>
#include <sys/types.h>

#define RESPONSE_BUFFSIZE 128

class Response {
public:
  // Response();
  // Response(const Response&);
  // Response& operator=(const Response&);
  // ~Response();

  explicit Response(const Request &Req);

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
  typedef enum { SendFile, ReceiveFile, Cgi } ProcessType;

  bool init(const Request &Req);

  // sending files + metadata
  bool process(const int Socket, const size_t Bytes);
  bool sendFile(const int Socket, const size_t Bytes);

  bool initError(const int Code);
  void initSendFile(const int Code, const char *File);

  bool makeMetadata(const int Code);
  bool sendMetadata(const int Socket, const size_t Bytes);
  bool processDelete(const int, const size_t);
  bool sendBuffer(const int, const size_t);
  bool fillBufferFile(const size_t);

  HttpHeaders _headers;

  ProcessType _ptype;

  // consider abstraction for metaData
  bool _hasMetadata;
  bool _metaDataSent;
  std::string _metaData;

  int _fdIn;
  int _fdOut;

  // consider abstraction for buffer
  Buffer _buffer;
};
