#include "Response.hpp"

#include "Logging.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

Response::Response(const Request &Req)
  : _ptype(None), _metadataSent(false), _fdIn(-1), _fdOut(-1) {

  logging::log2(logging::Debug, __func__, " called");
	if (!Req.isValid()) {
		initSendFile(CODE_400, FILE_400);
    return;
  }

	// make more generic
	if (Req.getMajorV() != 1 || Req.getMinorV() != 0) {
		initSendFile(CODE_501, FILE_501); // correct?
    return;
  }

	try {
		switch (Req.getMethod()) {
			case Get:
				initSendFile(CODE_200, Req.getResource().c_str());
        return;
			case Post:
			case Delete:
        initSendFile(CODE_501, FILE_501);
        return;
		}
	} catch (...) {
		initSendFile(CODE_500, FILE_500);
    return;
	}
}

void Response::initSendFile(const int Code, const char *File) {

  struct stat statbuf;
  if (File != NULL) {
    if (stat(File, &statbuf) < 0) {
      if (Code == CODE_500)
        return initSendFile(CODE_500, NULL);
      return initSendFile(CODE_500, FILE_500);
    }
  }
	
	_fdIn = open(File, O_RDONLY);
	if (_fdIn < 0) {
    if (Code == CODE_500)
      return initSendFile(CODE_500, NULL);
    return initSendFile(CODE_500, FILE_500);
  }

  if (File != NULL)
    _headers.setContentLength(statbuf.st_size);

  std::ostringstream oss;
  oss << _headers;
  _metadata = oss.str();
	_fdOut = -1;
  _ptype = SendFile;
  _metadataSent = false;
}
