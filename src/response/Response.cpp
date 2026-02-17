#include "Response.hpp"

#include "Logging.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include <cerrno>
#include <cstddef>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>

static std::string getReasonPhrase(const int Code);

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

  switch (Req.getMethod()) {
    case Get:
      initSendFile(CODE_200, Req.getResource().c_str());
      return;
    case Post:
    case Delete:
    case Generic:
      initSendFile(CODE_501, FILE_501);
      return;
		}
}

bool Response::statbufPopulate(const int Code, const char *File,
          struct stat &statbuf) {
  if (File == NULL)
    return true;
  if (stat(File, &statbuf) == 0)
    return true;
  if (Code == CODE_500) {
    errno = 0;
    initSendFile(CODE_500, NULL);
  }
  const int err = errno;
  errno = 0;
  return initError(err);
}

bool Response::setFdIn(const int Code, const char *File) {
  if (File == NULL)
    return true;
  _fdIn = open(File, O_RDONLY);
	if (_fdIn >= 0)
    return true;
  const int err = errno;
  errno = 0;
  return initError(err);
}

// TODO:
// Check if recursion is safe and
// potentially disable clang-tidy for this part.
// think about splitting up
void Response::initSendFile(const int Code, const char *File) {

  struct stat statbuf;
  if (!statbufPopulate(Code, File, statbuf))
    return;
	
  if (!setFdIn(Code, File))
    return;

  if (File != NULL)
    _headers.setContentLength(statbuf.st_size);

  std::ostringstream oss;
  oss << "HTTP/1.0 " << Code << ' ' << getReasonPhrase(Code) << "\r\n";
  oss << _headers << "\r\n";
  _metadata += oss.str();
	_fdOut = -1;
  _ptype = SendFile;
  _metadataSent = false;
}

static std::string getReasonPhrase(const int Code) {
  switch (Code) {
    case CODE_200: return REASON_200;
    case CODE_201: return REASON_201;
    case CODE_202: return REASON_202;
    case CODE_204: return REASON_204;
    case CODE_301: return REASON_301;
    case CODE_302: return REASON_302;
    case CODE_304: return REASON_304;
    case CODE_400: return REASON_400;
    case CODE_401: return REASON_401;
    case CODE_403: return REASON_403;
    case CODE_404: return REASON_404;
    case CODE_500: return REASON_500;
    case CODE_501: return REASON_501;
    case CODE_502: return REASON_502;
    case CODE_503: return REASON_503;
    default: return "";
  }
}

bool Response::initError(const int Errno) {
  switch (Errno) {
    case EACCES: initSendFile(CODE_401, FILE_401); return false;
    case ENOENT: initSendFile(CODE_404, FILE_404); return false;
    default: initSendFile(CODE_500, FILE_500); return false;
  }
}
