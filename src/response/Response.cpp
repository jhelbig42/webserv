#include "ReasonPhrases.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <fcntl.h>
#include <sys/stat.h>

Response::Response(Request &Req)
  : _ptype(None), _metaDataSent(false), _fdIn(-1), _fdOut(-1) {
}

/// @brief initializes a processingState object so it can be used for chunkwise processing
///
/// checked runtime errors:
/// semantic falsety of Req
/// system call failure
///
/// unchecked runtime errors:
/// syntactic falsety of Req
///
/// side effects:
/// rewrites all attributes of object
/// might open files
/// might delete files
///
/// @param Req provides information for initialization
bool Response::init(const Request &Req)
{
  _metaDataSent = false;
  _metaData = "";
  _fdIn = -1;
  _fdOut = -1;
  _eof = false;

	if (!Req.isValid())
		return initSendFile(CODE_400, FILE_400);

	// make more generic
	if (Req.getMajorV() != 1 || Req.getMinorV() != 0)
		return initError(CODE_501); // correct?

	try {
		switch (_req.getMethod()) {
			case Get:
				return initGet();
			case Post:
			case Delete:
      case Generic:
        return initError(CODE_501);
		}
	} catch (...) {
		return initError(CODE_500); // cases
	}
}

void Response::initSendFile(const int Code, const char *File) {

  struct stat statbuf;
  if (File != NULL) {
    if (stat(File, &statbuf) < 0) {
      if (Code == CODE_500)
        return initSendFile(CODE_500, NULL)
      return initSendFile(CODE_500, FILE_500);
    }
  }
	
	_fdIn = open(File, O_RDONLY);
	if (_fdIn < 0) {
    if (Code == CODE_500)
      return initSendFile(CODE_500, NULL)
    return initSendFile(CODE_500, FILE_500);
  }

  if (File != NULL)
    setContentLength = statbuf.st_size;

	_fdOut = -1;
  _ptype = SendFile;
  _metaDataSent = false;
  return true;
}

Response::Response(const Request &Req): _req(Req) {
  init(Req);
}
