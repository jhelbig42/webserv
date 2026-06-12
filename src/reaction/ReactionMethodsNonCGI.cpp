#include "Autoindex.hpp"
#include "HttpMethods.hpp"
#include "Logging.hpp"
#include "Reaction.hpp"
#include "Request.hpp"
#include "StatusCodes.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

void Reaction::initMethodNonCGI(const Request &Req) {
  logging::log3(logging::Debug, "Reaction: ", __func__, " called");
  switch (Req.getMethod()) {
  case Head:
  case Get:
    initHeadGet(Req);
    return;
  case Delete:
    initDelete();
    return;
  case Post:
    initSendError(CODE_403);
    return;
  case Generic:
    initSendError(CODE_501);
    return;
  }
}

void Reaction::initDelete(void) {
  errno = 0;
  if (std::remove(_pathInfo.getRealPath().c_str()) != 0) {
    initError(errno);
    return;
  }
  initSendFile(CODE_202, NULL);
}

void Reaction::initHeadGet(const Request &Req) {
  const std::string &path = _pathInfo.getRealPath();
  struct stat statbuf;

  if (stat(path.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
    if (path.empty() || path[path.size() - 1] != '/') {
      initSendError(CODE_404);
      return;
    }
    const std::list<std::string> &indexFiles = _pathInfo.getIndex();
    for (std::list<std::string>::const_iterator it = indexFiles.begin();
         it != indexFiles.end(); ++it) {
      if (access(it->c_str(), F_OK) == 0) {
        initSendFile(CODE_200, it->c_str());
        if (Req.getMethod() == Head && _fdIn >= 0) {
          if (close(_fdIn) < 0)
            logging::log2(logging::Error, "close: ", strerror(errno));
          _fdIn = -1;
        }
        return;
      }
    }
    if (_pathInfo.getAutoindex()) {
      std::string dir = path;
      if (dir.empty() || dir[dir.size() - 1] != '/')
        dir += '/';
      Autoindex ai;
      std::string const html = ai.autoindexStream(dir, Req.getResource());
      if (ai.getErrCode() != CODE_200) {
        initSendError(ai.getErrCode());
        return;
      }
      initSendString(CODE_200, html);
      if (Req.getMethod() == Head)
        _body.clear();
      return;
    }
    initSendError(CODE_403);
    return;
  }

  if (access(path.c_str(), F_OK) != 0) {
    initSendError(CODE_404);
    return;
  }
  if (access(path.c_str(), R_OK) != 0) {
    initSendError(CODE_403);
    return;
  }

  initSendFile(CODE_200, path.c_str());
  if (Req.getMethod() == Get || _fdIn < 0)
    return;
  errno = 0;
  if (close(_fdIn) < 0)
    logging::log2(logging::Error, "close: ", strerror(errno));
  _fdIn = -1;
}

void Reaction::setTmpPathName(void) {
  std::stringstream sname;
  sname << _finalPath << _sock;
  _tmpPath = sname.str();
}

// also needs the Post path from config
void Reaction::setFinalPathName(void) {
  _finalPath = _pathInfo.getRealPath();
}
