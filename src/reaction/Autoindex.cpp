#include "Autoindex.hpp"
#include "StatusCodes.hpp"
#include "Time.hpp"
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

Autoindex::Autoindex() : _errCode(CODE_200) {}

Autoindex::~Autoindex() {}

Autoindex::Autoindex(const Autoindex &Other) : _errCode(Other._errCode) {}

Autoindex &Autoindex::operator=(const Autoindex &Other) {
	_errCode = Other._errCode;
	return *this;
}

int Autoindex::getErrCode() const {
	return _errCode;
}

std::vector<std::string> Autoindex::createListing(const std::string &DirectoryName){
	DIR *dirp = opendir(DirectoryName.c_str());
	if (!dirp) {
		_errCode = CODE_500;
	}

	std::vector<std::string> names;
	errno = 0;
	dirent *entry;
	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_name[0] != '.')
			names.push_back(entry->d_name);
	}
	closedir(dirp);

	if (errno) {
		_errCode = CODE_500;
	}
	std::sort(names.begin(), names.end());

	return(names);
}

std::string Autoindex::createHTML(const std::string &DirectoryName, const std::string &RequestedPath, const std::vector<std::string> &Names){
	std::stringstream stream;

	stream << "<html><head><title>Index of " << RequestedPath << "</title></head>\n"
	   << "<body><h1>Index of " << RequestedPath << "</h1><hr><pre>"
	   << "<a href=\"../\">../</a>\n";

	struct stat entryStat;
	for (size_t i = 0; i < Names.size(); ++i) {
		std::string const pathname = DirectoryName + Names[i];
		if (stat(pathname.c_str(), &entryStat)) {
			_errCode = CODE_500;
			return stream.str();
		}

		std::string displayName;
		bool const isDir = S_ISDIR(entryStat.st_mode);
		if (isDir)
			displayName = Names[i] + "/";
		else
			displayName = Names[i];

		stream << "<a href=\"" << displayName << "\">" << displayName << "</a>";

		int pad = 50 - static_cast<int>(displayName.size());
		if (pad < 1)
			pad = 1;
		stream << std::string(pad, ' ');

		stream << getTimeStringFromTimespec(entryStat.st_mtim);

		if (isDir)
			stream << std::setw(20) << "-";
		else
			stream << std::setw(20) << entryStat.st_size;
		stream << "\n";
	}
	stream << "</pre><hr></body>\n</html>\n";
	return stream.str();
}

std::string Autoindex::autoindexStream(const std::string &DirectoryName, const std::string &RequestedPath) {

	std::vector<std::string> const names = createListing(DirectoryName);

	if (_errCode != CODE_200)
		return "";

	std::string const html = createHTML(DirectoryName, RequestedPath, names);

	if (_errCode != CODE_200)
		return "";

	return html;
}
