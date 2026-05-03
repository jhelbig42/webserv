
#include "CGIProcess.hpp"
#include "HttpHeaders.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include "Website.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>

static const char *defaultErrorFile(int Code);
static std::string getReasonPhrase(const int Code);
static bool hasDefaultFile(const int Code);
static void setMetadata(std::string &Metadata, const int Code,
                        const HttpHeaders &Hdrs);

void Reaction::setDefaults(void) {
  _processType = NotInitialized;
  _metadataSent = false;
  _fdIn = -1;
  _headers.unsetAll();  
}

Reaction::Reaction()
    : _processType(NotInitialized), _metadataSent(false), _fdIn(-1) {
  _headers.unsetAll();
}


Reaction::ProcessType Reaction::getProcessType(void) const{
  return _processType;
}

int Reaction::getForwardSocket(void) const {
  return _cgi.getForwardSocket();
}

void Reaction::setPathInfo(const PathInfo &PathInfo){
	_pathInfo = PathInfo;
}

bool Reaction::isCGI(const Request &Req){
  if (Req.getHeaders().getContentType() == HttpHeaders::ApplicationSh ||
		Req.getHeaders().getContentType() == HttpHeaders::TextPython)
  {
    return true;
  }
  return false;
}

void Reaction::init(const Request &Req, const int Socket) {
  setDefaults();
  _sock = Socket;

  logging::log3(logging::Debug, "Reaction::", __func__, " called");
  if (Req.getState() == INVALID) {
    initSendFile(CODE_400, getErrorFile(CODE_400).c_str());
    return;
  }
  if (!(_pathInfo.getAllowed() & Req.getMethod())){
	logging::log(logging::Debug, "Requested method not allowed");
  	initSendFile(CODE_403, getErrorFile(CODE_403).c_str());
	return ;
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
  if(_pathInfo.getCgiPath() == ""){
	  logging::log(logging::Debug, "Req is NOT a CGI");
	  if (Req.getQueryString() != ""){ // query strings are just allowed in CGI calls
		  initSendFile(CODE_400, getErrorFile(CODE_400).c_str());
		  return;
	  }
	  initMethodNonCGI(Req);
	  return;
  }

  logging::log(logging::Debug, "Req is a CGI");
  _cgi.setCGIPath(_pathInfo.getCgiPath());
  if (!_cgi.init(Req, _script, _pathInfo.getRealPath())) {
    initSendFile(CODE_500, getErrorFile(CODE_500).c_str());
    return;
  }
  initCGIMethod(Req);
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
}


static void setMetadata(std::string &Metadata, const int Code,
                        const HttpHeaders &Hdrs) {
  std::ostringstream oss;
  oss << "HTTP/1.0 " << Code << ' ' << getReasonPhrase(Code) << "\r\n";
  oss << Hdrs << "\r\n";
  Metadata = oss.str();
}

bool Reaction::fallbackOrError(const int Code) {
  if (hasDefaultFile(Code)) {
    logging::log3(logging::Warning, "Code ", Code,
                  ": Default file not accessible. Only sending status line.");
    initSendFile(Code, NULL);
    return false;
  }
  return initError(errno);
}

bool Reaction::statbufPopulate(const int Code, const char *File,
                               struct stat &StatBuf) {
  if (File == NULL)
    return true;
  errno = 0;
  if (stat(File, &StatBuf) == 0)
    return true;
  return fallbackOrError(Code);
}

bool Reaction::setFdIn(const int Code, const char *File) {
  if (File == NULL)
    return true;
  errno = 0;
  _fdIn = open(File, O_RDONLY);
  if (_fdIn >= 0)
    return true;
  return fallbackOrError(Code);
}

bool Reaction::initPostBody(const Request &Req) {
  if (!Req.getHeaders().isSet(HttpHeaders::ContentLength)) {
    logging::log(logging::Debug, "POST: Content-Length header missing");
    initSendFile(CODE_400, FILE_400);
    return false;
  }
  _reqContLen = Req.getHeaders().getContentLength();
  if (_reqContLen > _pathInfo.getMaxReqBody()) {
    logging::log(logging::Debug, "requested Content Length exceeds Max Body Length allowed by Config");
    initSendFile(CODE_403, getErrorFile(CODE_403).c_str());
    return false;
  }
  _receivedContLen = 0;
  _buffer = Req.getBuffer();
  return true;
}

static bool hasDefaultFile(const int Code) {
  return Code != CODE_200;
}

bool Reaction::initError(const int Errno) {
  switch (Errno) {
  case EACCES:
    initSendFile(CODE_403, getErrorFile(CODE_403).c_str());
    return false;
  case ENOENT:
    initSendFile(CODE_404, getErrorFile(CODE_404).c_str());
    return false;
  default:
    initSendFile(CODE_500, getErrorFile(CODE_500).c_str());
    return false;
  }
}

static const char *defaultErrorFile(int Code) {
  switch (Code) {
  case CODE_400: 
    return FILE_400;
  case CODE_401: 
    return FILE_401;
  case CODE_403: 
    return FILE_403;
  case CODE_404: 
    return FILE_404;
  case CODE_500: 
    return FILE_500;
  case CODE_501: 
    return FILE_501;
  case CODE_502: 
    return FILE_502;
  case CODE_503: 
    return FILE_503;
  default:       
    return NULL;
  }
}

std::string Reaction::getErrorFile(int Code) const {
  const char *configFile = _pathInfo.getErrorPage(static_cast<unsigned int>(Code));
  if (configFile != NULL) {
    if (configFile[0] == '/')
      return (_pathInfo.getRoot() + (configFile + 1)); // jump over '/'
    return configFile;
  }
  return defaultErrorFile(Code); // just if no specific file was found in config
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
