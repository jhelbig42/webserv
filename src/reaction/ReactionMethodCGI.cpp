#include "HttpMethods.hpp"
#include "Reaction.hpp"
#include "Request.hpp"

void Reaction::initCGIMethod(const Request &Req) {
  logging::log2(logging::Debug, __func__, "trying to access the scriptfile");
  logging::log(logging::Debug, _pathInfo.getCgiPath().c_str());
  errno = 0;
  if (access(_pathInfo.getRealPath().c_str(), F_OK | R_OK) != 0) {
    logging::log(logging::Debug, "error in CGI path");
    if (errno == ENOENT)
      initSendError(404);
    else if (errno == EACCES)
      initSendError(403);
    else
      throw std::runtime_error(strerror(errno));
    return;
  }
  if (access(_pathInfo.getCgiPath().c_str(), F_OK | X_OK) != 0) {
    logging::log(logging::Debug, "error in script path");
    if (errno == ENOENT)
      initSendError(404);
    else if (errno == EACCES)
      initSendError(403);
    else
      throw std::runtime_error(strerror(errno));
    return;
  }

  if (Req.getMethod() == Post) {
    if (!initPostBody(Req))
      return;
    _processType = CgiPost;
    return;
  }
  _cgi.setInputDone(true); // Here, setInputDone is set to true because Method is GET & there is nothing to send
  _processType = CgiNotPost;
}
