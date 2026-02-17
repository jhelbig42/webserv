#include "ReasonPhrases.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <fcntl.h>
#include <sys/stat.h>

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
  _method = Generic;
  _contentLength = 0;
  _hasMetadata = false;
  _metaDataSent = false;
  _metaData = "";
  _fdIn = -1;
  _fdOut = -1;
  _eof = false;

	if (!Req.isValid())
		return initError(CODE_400);

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
	
	_fdIn = open(_req.getResource().c_str(), O_RDONLY);
	if (_fdIn < 0)
		return initError(CODE_500);

	_contentLength = statbuf.st_size;
  _hasMetadata = false;
	_fdOut = -1;
  _eof = false;
  return true;
}

Response::Response(const Request &Req): _req(Req) {
  init(Req);
}

bool Response::initError(const int Code)
{
  makeMetadata(Code);
  _hasMetadata = true;
  return true;
}

/// TODO: check error handling
bool Response::initGet()
{
	struct stat statbuf;
	if (stat(_req.getResource().c_str(), &statbuf) < 0)
		return initError(CODE_500);
	
	_fdIn = open(_req.getResource().c_str(), O_RDONLY);
	if (_fdIn < 0)
		return initError(CODE_500);

	_contentLength = statbuf.st_size;
  _hasMetadata = false;
	_fdOut = -1;
  _eof = false;
  return true;
}
