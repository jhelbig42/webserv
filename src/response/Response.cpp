#include "Response.hpp"
#include "Request.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

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
  _bufStart = 0;
  _bufEnd = 0;
  _eof = false;

	if (!Req.isValid())
		return initError(400);

	// make more generic
	//if (Req.getMajorV() != 1 || Req.getMinorV() != 0)
	//	return initError(501); // correct?

	try {
		switch (_req.getMethod()) {
			case Get:
				return initGet();
			case Post:
        return initError(501);
			case Delete:
        return initError(501);
      case Generic:
        return initError(501);
		}
	} catch (...) {
		return initError(500); // cases
	}
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
		return initError(500);
	
	if ((_fdIn = open(_req.getResource().c_str(), O_RDONLY)) < 0)
		return initError(500);

	_contentLength = statbuf.st_size;
	_bufStart = 0;
	_bufEnd = 0;
  _hasMetadata = false;
	_fdOut = -1;
  _eof = false;
  return true;
}
