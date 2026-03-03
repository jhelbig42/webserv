#pragma once

#include "Buffer.hpp"
#include "HttpHeaders.hpp"
#include "Request.hpp"
#include <string>
#include <sys/types.h>

class Reaction {
public:
  Reaction();
  // Reaction(const Reaction&);
  // Reaction& operator=(const Reaction&);
  // ~Reaction();

  typedef enum { None, SendFile, ReceiveFile, Cgi } ProcessType;

  explicit Reaction(const Request &Req);
  void init(const Request &Req);

  /// \fn bool process(const int Socket, const size_t Bytes);
  /// \brief continues processing a Reaction object
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
  /// \param Socket socket associated with Reaction
  /// \param Bytes the maximum amount of Bytes to process by system calls
  ///
  /// \return true if Reaction got fully processed otherwise false
  bool process(const int Socket, int &ForwardSocket, const size_t Bytes);

  Conditions getConditions(void) const;

private:
  // sending files + metadata
  bool sendFile(const int Socket, const size_t Bytes);
  void initSendFile(const int Code, const char *File);
  bool statbufPopulate(const int Code, const char *File, struct stat &StatBuf);
  bool setFdIn(const int Code, const char *File);
  bool initError(const int Errno);
  void setDefaults(void);
  void initMethod(const Request &Req);
  void initHeadGet(const Request &Req);
  void initDelete(const Request &Req);

  Conditions _conditions;

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
