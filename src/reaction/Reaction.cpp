
#include "CGIProcess.hpp"
#include "Conditions.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
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

static std::string getReasonPhrase(const int Code);
static bool hasDefaultFile(const int Code);
static void setMetadata(std::string &Metadata, const int Code,
                        const HttpHeaders &Hdrs);

void Reaction::setDefaults(void) {
  _processType = NotInitialized;
  _metadataSent = false;
  _fdIn = -1;
  _headers.unsetAll();
  _conditions = Unconditional;
}

Reaction::Reaction()
    : _processType(NotInitialized), _metadataSent(false), _fdIn(-1) {
  _headers.unsetAll();
}

Conditions Reaction::getConditions(void) const {
  return _conditions;
}

Reaction::ProcessType Reaction::getProcessType(void) const{
  return _processType;
}

int Reaction::getForwardSocket(void) const {
  return _cgi.getForwardSocket();
}

bool		Reaction::isCGI(const Request &Req){
  if (Req.getHeaders().getContentType() == HttpHeaders::ApplicationSh ||
		Req.getHeaders().getContentType() == HttpHeaders::TextPython)
  {
    _processType = Cgi;
    return true;
  }
  return false;
}

void Reaction::init(const Request &Req) {
  setDefaults();

  logging::log3(logging::Debug, "Reaction::", __func__, " called");
  if (Req.getState() == INVALID) {
    initSendFile(CODE_400, FILE_400);
    return;
  }

  // TODO: make more generic
  /*
  if (Req.getMajorV() != 1 || Req.getMinorV() != 0) {
    initSendFile(CODE_501, FILE_501);
    return;
  }
  */ // commenting out because we need to allow 1.1 requests
  // and can response with 1.0
  //
  //check if method is allowed in comparison to config

  //check if Resource is a CGI script
  //processType CGI

  //set request Type header just here, safe Query string, if we have one
  //invalid request if we have a query string, but not a script we asked for
  if(!isCGI(Req)){
	  logging::log(logging::Debug, "Req is NOT a CGI");
	  if (Req.getQueryString() != ""){
		  initSendFile(CODE_400, FILE_400);
		  return;
	  }
	  initMethodNonCGI(Req);
	  return;
  }
  logging::log(logging::Debug, "Req is a CGI");
  if (!_cgi.init(Req, _script)) {
    initSendFile(CODE_500, FILE_500);
    return;
  }
  if (Req.getMethod() == Post) {
    if (!Req.getHeaders().isSet(HttpHeaders::ContentLength)) {
      logging::log(logging::Debug, "CGI POST: Content-Length header missing");
      initSendFile(CODE_400, FILE_400);
      return;
    }
    _reqContLen = Req.getHeaders().getContentLength();
    _receivedContLen = 0;
    _buffer = Req.getBuffer();
    _conditions = FSockWrite;
  }
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
    _headers.setContentLength(static_cast<size_t>(statbuf.st_size));
    _headers.setContentType(strrchr(File, '.'));
  }

  setMetadata(_metadata, Code, _headers);
  _metadataSent = false;
  _processType = SendFile;
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
