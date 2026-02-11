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

bool Response::initGet(const Request &Req)
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
