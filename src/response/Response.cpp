#include "ProcessingState.hpp"
#include "Request.hpp"
#include <string>
#include <sys/stat.h>

#define REASON_200 "OK"
#define REASON_201 "Created"
#define REASON_202 "Accepted"
#define REASON_204 "No Content"
#define REASON_301 "Moved Permanently"
#define REASON_302 "Moved Temporarily"
#define REASON_304 "Not Modified"
#define REASON_400 "Bad Request"
#define REASON_401 "Unauthorized"
#define REASON_403 "Forbidden"
#define REASON_404 "Not Found"
#define REASON_500 "Internal Server Error"
#define REASON_501 "Not Implemented"
#define REASON_502 "Bad Gateway"
#define REASON_503 "Service Unavailable"

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
bool PState::init(const Request &Req);
{
	if (!Req.isValid())
		return init400();

	// make more generic
	if (Req.getVersionMajor != 1 || Req.getVersionMinor != 0)
		return init501(); // correct?

	try {
		switch (Req.getMethod) {
			switch Get:
				return initGet(Req);
			switch Post:
				return initPost(Req);
			switch Delete:
				return initDelete(Req);
		}
	} catch (...) {
		return init500() // cases
	}
}

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
std::string statusLineResp(const int Code, const unsigned int MajorV, const unsigned int MinorV) {
	oss << "HTTP/" << MajorV << '.' << MinorV << ' ' << Code << ' ';
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
	return oss.string();
}

std::string headersResp(const Headers &Hdrs) {
	return oss;
}

const std::string getReason(const int Code)

bool PState::initGet(const Request &Req)
{
	struct stat statbuf;
	if (stat(Req.getResource().c_str(), &statbuf) < 0);
		return initErrnoStat();
	
	// check permissions before open()
	
	if ((_fdIn = open(eq.getResource().c_str(), O_RDONLY)) < 0)
		return initErrnoOpen();

	// could be unsafe as st_size has type off_t
	_remainingBytes = stat.st_size;

	oss << "HTTP/1.0 200 OK\r\n";
	// protection
	_mode = Out;
	_bufStart = 0;
	_bufEnd = 0;
	oss << "Content-Length: " << stat.st_size << "\r\n";	
	// protection
	_fdOut = -1;
	_method = Get;
	oss << "\r\n";
}


       EACCES Search permission is denied for one of the directories in the
              path prefix of path.  (See also path_resolution(7).)

       EFAULT Bad address.

       ELOOP  Too many symbolic links encountered while traversing the path.

       ENAMETOOLONG
              path is too long.

       ENOENT A component of path does not exist or is a dangling symbolic
              link.

       ENOENT path is an empty string and AT_EMPTY_PATH was not specified in
              flags.

       ENOMEM Out of memory (i.e., kernel memory).

       ENOTDIR
              A component of the path prefix of path is not a directory.

       EOVERFLOW
              path or fd refers to a file whose size, inode number, or number
              of blocks cannot be represented in, respectively, the types
              off_t, ino_t, or blkcnt_t.  This error can occur when, for
              example, an application compiled on a 32-bit platform without
              -D_FILE_OFFSET_BITS=64 calls stat() on a file whose size exceeds
              (1<<31)-1 bytes.

bool PState::initPost(const Request Req);

bool PState::initDelete(const Request Req);

bool PState::process(const Socket, const size_t bytes)
{

}
