#pragma once

#include <string>
#include <vector>

class Autoindex {
private:
	int _errCode;

	std::vector<std::string> createListing(const std::string &DirectoryName);
	std::string createHTML(const std::string &DirectoryName, const std::vector<std::string> &names);
public:
	Autoindex();
	Autoindex(const Autoindex &Other);
	Autoindex &operator=(const Autoindex &Other);
	~Autoindex();

	std::string AutoindexStream(const std::string &DirectoryName);
	int getErrCode() const;
};
