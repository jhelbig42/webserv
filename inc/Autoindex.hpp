#pragma once

#include <string>
#include <vector>

class Autoindex {
private:
	int _errCode;

	std::vector<std::string> createListing(const std::string &DirectoryName);
	std::string createHTML(const std::string &DirectoryName, const std::string &RequestedPath, const std::vector<std::string> &Names);
public:
	Autoindex();
	Autoindex(const Autoindex &Other);
	Autoindex &operator=(const Autoindex &Other);
	~Autoindex();

	std::string autoindexStream(const std::string &DirectoryName, const std::string &RequestedPath);
	int getErrCode() const;
};
