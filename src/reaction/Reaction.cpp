#include "Reaction.hpp"

#include "Conditions.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

static std::string getReasonPhrase(const int Code);
static bool hasDefaultFile(const int Code);
static void setMetadata(std::string &Metadata, const int Code,
                        const HttpHeaders &Hdrs);

void Reaction::setDefaults(void) {
  _ptype = None;
  _metadataSent = false;
  _fdIn = -1;
  _fdOut = -1;
  _headers.unsetAll();
  _conditions = Unconditional;
}

Reaction::Reaction()
    : _ptype(None), _metadataSent(false), _fdIn(-1), _fdOut(-1) {
  _headers.unsetAll();
}

Conditions Reaction::getConditions(void) const {
  return _conditions;
}

void Reaction::init(const Request &Req) {
  setDefaults();

  logging::log3(logging::Debug, "Reaction: ", __func__, " called");
  if (Req.getState() == INVALID) {
    initSendFile(CODE_400, FILE_400);
    return;
  }

  // TODO: make more generic
  if (Req.getMajorV() != 1 || Req.getMinorV() != 0) {
    initSendFile(CODE_501, FILE_501);
    return;
  }

  initMethod(Req);
}

void Reaction::initMethod(const Request &Req) {
  switch (Req.getMethod()) {
  case Head:
  case Get:
    initHeadGet(Req);
    return;
  case Delete:
    initDelete(Req);
    return;
  case Post:
	initPost(Req);
	return;
  case Generic:
    initSendFile(CODE_501, FILE_501);
    return;
  }
}

void Reaction::initDelete(const Request &Req) {
  errno = 0;
  if (std::remove(Req.getResource().c_str()) != 0) {
    initError(errno);
    return;
  }
  initSendFile(CODE_202, NULL);
}

void Reaction::initHeadGet(const Request &Req) {
  initSendFile(CODE_200, Req.getResource().c_str());
  if (Req.getMethod() == Get || _fdIn < 0)
    return;
  errno = 0;
  if (close(_fdIn) < 0)
    logging::log2(logging::Error, "close: ", strerror(errno));
  _fdIn = -1;
}

Reaction::Reaction(const Request &Req)
    : _ptype(None), _metadataSent(false), _fdIn(-1), _fdOut(-1) {
  init(Req);
}

// TODO:
// Check if recursion is safe and
// potentially disable clang-tidy for this part.
// think about splitting up
void Reaction::initSendFile(const int Code, const char *File) {
  struct stat statbuf;
  if (!statbufPopulate(Code, File, statbuf))
    return;

  if (!setFdIn(Code, File))
    return;

  if (File != NULL) {
    _headers.setContentLength(statbuf.st_size);
    _headers.setContentType(strrchr(File, '.'));
  }

  setMetadata(_metadata, Code, _headers);
  _metadataSent = false;
  _fdOut = -1;
  _ptype = SendFile;
  _conditions = SockWrite;
}

static void setMetadata(std::string &Metadata, const int Code,
                        const HttpHeaders &Hdrs) {
  std::ostringstream oss;
  oss << "HTTP/1.0 " << Code << ' ' << getReasonPhrase(Code) << "\r\n";
  oss << Hdrs << "\r\n";
  Metadata = oss.str();
}

bool Reaction::statbufPopulate(const int Code, const char *File,
                               struct stat &StatBuf) {
  if (File == NULL)
    return true;
  errno = 0;
  if (stat(File, &StatBuf) == 0)
    return true;
  if (hasDefaultFile(Code)) {
    logging::log3(logging::Warning, "Code ", Code,
                  ": Default file not accessible. Only sending status line.");
    initSendFile(Code, NULL);
    return false;
  }
  return initError(errno);
}

bool Reaction::setFdIn(const int Code, const char *File) {
  if (File == NULL)
    return true;
  errno = 0;
  _fdIn = open(File, O_RDONLY);
  if (_fdIn >= 0)
    return true;
  if (hasDefaultFile(Code)) {
    logging::log3(logging::Warning, "Code ", Code,
                  ": Default file not accessible. Only sending status line.");
    initSendFile(Code, NULL);
    return false;
  }
  return initError(errno);
}

static bool hasDefaultFile(const int Code) {
  return Code != CODE_200;
}

bool Reaction::initError(const int Errno) {
  switch (Errno) {
  case EACCES:
    initSendFile(CODE_403, FILE_403);
    return false;
  case ENOENT:
    initSendFile(CODE_404, FILE_404);
    return false;
  default:
    initSendFile(CODE_500, FILE_500);
    return false;
  }
}

static std::string getReasonPhrase(const int Code) {
  switch (Code) {
  case CODE_200:
    return REASON_200;
  case CODE_201:
    return REASON_201;
  case CODE_202:
    return REASON_202;
  case CODE_204:
    return REASON_204;
  case CODE_301:
    return REASON_301;
  case CODE_302:
    return REASON_302;
  case CODE_304:
    return REASON_304;
  case CODE_400:
    return REASON_400;
  case CODE_401:
    return REASON_401;
  case CODE_403:
    return REASON_403;
  case CODE_404:
    return REASON_404;
  case CODE_500:
    return REASON_500;
  case CODE_501:
    return REASON_501;
  case CODE_502:
    return REASON_502;
  case CODE_503:
    return REASON_503;
  default:
    return "";
  }
}
