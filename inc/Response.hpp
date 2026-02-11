#pragma once

#include "Request.hpp"
#include <string>

#define RESPONSE_BUFFSIZE 128

class Response: {
public:
  // Response();
  // Response(const Response&);
  // Response& operator=(const Response&);
  // ~Response();

  bool init(const Request &);

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

  bool processHead(const int, const size_t);
  bool makeMetadata(const int Code, const Request &req);
  bool sendMetadata(); // still a dummy
  bool processGet(const int, const size_t);
  bool processDelete(const int, const size_t);
  bool sendBuffer(const int, const size_t);
  bool fillBufferFile(const size_t);

  Request _req;

  Headers _headers;

  // consider abstraction for metaData
  bool _hasMetadata;
  bool _metaDataSent;
  std::string _metaData;

  int _fdIn;
  int _fdOut;

  // consider abstraction for buffer
  char _buffer[RESPONSE_BUFFSIZE];
  size_t _buffSize = RESPONSE_BUFFSIZE;
  size_t _bufStart;
  size_t _bufEnd;

  // consider getting rid of _eof
  // as the information should already be included int _remainingFileSize 
  bool _eof;

  size_t _remainingFileSize;
};
