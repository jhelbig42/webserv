
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

static std::string getReasonPhrase(const int Code);
static bool hasDefaultFile(const int Code);
static void setMetadata(std::string &Metadata, const int Code,
                        const HttpHeaders &Hdrs);

void Reaction::setDefaults(void) {
  _processType = NotInitialized;
  _http09 = false;
  _metadataSent = false;
  _fdIn = -1;
  _body.clear();
  _headers.unsetAll();
}

Reaction::Reaction()
    : _processType(NotInitialized), _http09(false), _metadataSent(false), _fdIn(-1) {
  _headers.unsetAll();
}


Reaction::ProcessType Reaction::getProcessType(void) const{
  return _processType;
}

int Reaction::getForwardSocket(void) const {
  return _cgi.getForwardSocket();
}

bool Reaction::getInputDone(void) const {
  return _cgi.getInputDone();
}

void Reaction::setPathInfo(const PathInfo &PathInfo){
	_pathInfo = PathInfo;
}

void Reaction::init(const Request &Req, const int Socket, int &ForwardSocket) {
  setDefaults();
  _sock = Socket;

  logging::log3(logging::Debug, "Reaction::", __func__, " called");
  if (Req.getState() == INVALID) {
    initSendError(CODE_400);
    return;
  }
  if (!(_pathInfo.getAllowed() & Req.getMethod())){
	logging::log(logging::Debug, "Requested method not allowed");
  	initSendError(CODE_403);
	return ;
  }
  
  _http09 = (Req.getMajorV() == 0 && Req.getMinorV() == 9);
  if (_http09) {
    if (Req.getMethod() != Get) {
      initSendError(CODE_400);
      return;
    }
  } else if (!((Req.getMinorV() == 0
        && (Req.getMajorV() == 1 || Req.getMajorV() == 2 || Req.getMajorV() == 3))
      || (Req.getMajorV() == 1 && Req.getMinorV() == 1))) {
    // allow 1.0, 2.0, 3.0 and 1.1
    initSendFile(CODE_501, FILE_501);
    return;
  }

  //check if Resource is a CGI script
  if(_pathInfo.getCgiPath() == ""){
	  logging::log(logging::Debug, "Req is NOT a CGI");
	  if (Req.getQueryString() != ""){ // query strings are just allowed in CGI calls
		  initSendError(CODE_400);
		  return;
	  }
	  initMethodNonCGI(Req);
	  return;
  }

  logging::log(logging::Debug, "Req is a CGI");
  initCGIMethod(Req);
  if (_processType != CgiPost && _processType != CgiNotPost)
    return;
  _cgi.setCGIPath(_pathInfo.getCgiPath());
  if (!_cgi.init(Req, _script, _pathInfo.getRealPath(), ForwardSocket)) {
    initSendError(CODE_500);
    return;
  }
}

void Reaction::initSendError(const int Code) {
  const char *configFile = _pathInfo.getErrorPage(static_cast<unsigned int>(Code));
  if (configFile != NULL) {
    std::string path;
    if (configFile[0] == '/')
      path = _pathInfo.getRoot() + (configFile + 1);
    else
      path = configFile;
    initSendFile(Code, path.c_str());
    return;
  }
  std::string phrase = getReasonPhrase(Code);
  std::ostringstream body;
  body << "<!DOCTYPE html>\r\n<html>\r\n<head><title>" << Code << " " << phrase
       << "</title></head>\r\n<body>\r\n<h1>" << Code << " " << phrase
       << "</h1>\r\n</body>\r\n</html>";
  initSendString(Code, body.str());
}

void Reaction::initSendString(const int Code, const std::string &Body) {
  _headers.setContentLength(Body.size());
  _headers.setContentType(".html");
  if (!_http09)
    setMetadata(_metadata, Code, _headers);
  _body = Body;
  _fdIn = -1;
  _metadataSent = _http09;
  _processType = SendFile;
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

  if (!_http09)
    setMetadata(_metadata, Code, _headers);
  _metadataSent = _http09;
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
    initSendError(CODE_411);
    return false;
  }
  _reqContLen = Req.getHeaders().getContentLength();
  if (_reqContLen > _pathInfo.getMaxReqBody()) {
    logging::log(logging::Debug, "requested Content Length exceeds Max Body Length allowed by Config");
    initSendError(CODE_403);
    return false;
  }
  _buffer = Req.getBuffer();
  _receivedContLen = _buffer.getUsed();
  return true;
}

static bool hasDefaultFile(const int Code) {
  return Code != CODE_200;
}

bool Reaction::initError(const int Errno) {
  switch (Errno) {
  case EACCES:
    initSendError(CODE_403);
    return false;
  case ENOENT:
    initSendError(CODE_404);
    return false;
  default:
    initSendError(CODE_500);
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
  case CODE_405:
    return REASON_405;
  case CODE_411:
    return REASON_411;
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
