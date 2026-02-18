#include "Response.hpp"
#include "ReasonPhrases.hpp"
#include "Request.hpp"
#include <sstream>
#include <string>

/// @brief returns a string that holds the status line of a response
///
/// unchecked runtime errors:
/// Code is not a status code explicitly listed in rfc1945 section 6.1.1
///
/// no sideeffects
///
/// @param Code http status code as in rfc1945
/// @param MajorV http major version
/// @param MinorV http minor version
static std::string statusLineResp(const int Code, const Request &Req);

static std::string headersResp(const off_t length);

bool Response::makeMetadata(const int Code) {
  _metaData = statusLineResp(Code, _req) + headersResp(_contentLength) + "\r\n";
  return true;
}

static std::string statusLineResp(const int Code, const Request &Req) {
  std::ostringstream oss;
	oss << "HTTP/" << Req.getMajorV() << '.';
  oss << Req.getMinorV() << ' ';
  oss << Code << ' ';
	switch (Code) {
		case 200: oss << REASON_200; break;
		case 201: oss << REASON_201; break;
		case 202: oss << REASON_204; break;
		case 204: oss << REASON_301; break;
		case 301: oss << REASON_301; break;
		case 302: oss << REASON_302; break;
		case 304: oss << REASON_304; break;
		case 400: oss << REASON_400; break;
		case 401: oss << REASON_401; break;
		case 403: oss << REASON_403; break;
		case 404: oss << REASON_404; break;
		case 500: oss << REASON_500; break;
		case 501: oss << REASON_501; break;
		case 502: oss << REASON_502; break;
		case 503: oss << REASON_503; break;
	}
	oss << "\r\n";
	return oss.str();
}

static std::string headersResp(const off_t length) {
  std::ostringstream oss;
  oss << "Content-Length: " << length << "\r\n";
	return oss.str();
}
