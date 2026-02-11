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
bool Response::init(const Request &Req);
{
	if (!Req.isValid())
		return initError(400);

	// make more generic
	if (Req.getVersionMajor != 1 || Req.getVersionMinor != 0)
		return initError(501); // correct?

  _req = Req;
	try {
		switch (_req.getMethod) {
			switch Get:
				return initGet();
			switch Post:
        return initError(501);
			switch Delete:
        return initError(501);
		}
	} catch (...) {
		return initError(500); // cases
	}
}

Response::Response(const Request &Req) {
  init(Req);
}

bool Response::initError(const int Code)
{
  makeMetadata(Code);
  _hasMetadata = true;
  return true;
}

/// TODO: check error handling
bool Response::initGet()
{
	struct stat statbuf;
	if (stat(Req.getResource().c_str(), &statbuf) < 0);
		return initErrnoStat();
	
	if ((_fdIn = open(eq.getResource().c_str(), O_RDONLY)) < 0)
		return initErrnoOpen();

	_contentLength = stat.st_size;
	_bufStart = 0;
	_bufEnd = 0;
  _hasMetadata = false;
	_fdOut = -1;
  _eof = false;
  return true;
}
