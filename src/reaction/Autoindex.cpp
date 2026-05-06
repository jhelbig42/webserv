#include "Autoindex.hpp"
#include "Logging.hpp"
#include "StatusCodes.hpp"
#include "Time.hpp"
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <iomanip>
#include <sstream>
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
	if (dirp == NULL) {
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

std::string Autoindex::createHTML(const std::string &DirectoryName, const std::vector<std::string> &names){
	std::stringstream stream;

	stream << "<html><head><title>Index of " << DirectoryName << "</title></head>\n"
	   << "<body><h1>Index of " << DirectoryName << "</h1><hr><pre>"
	   << "<a href=\"../\">../</a>\n";

	struct stat entryStat;
	for (size_t i = 0; i < names.size(); ++i) {
		std::string pathname = DirectoryName + names[i];
		if (stat(pathname.c_str(), &entryStat)) {
			_errCode = CODE_500;
			return stream.str();
		}

		std::string displayName;
		bool isDir = S_ISDIR(entryStat.st_mode);
		if (isDir)
			displayName = names[i] + "/";
		else
			displayName = names[i];

		stream << "<a href=\"" << displayName << "\">" << displayName << "</a>";

		int pad = 50 - static_cast<int>(displayName.size());
		if (pad < 1)
			pad = 1;
		stream << std::string(pad, ' ');

		stream << getTimeStringFromTimespec(entryStat.st_mtim, "%d-%b-%Y %H:%M");

		if (isDir)
			stream << std::setw(20) << "-";
		else
			stream << std::setw(20) << entryStat.st_size;
		stream << "\n";
	}
	stream << "</pre><hr></body>\n</html>\n";
	return stream.str();
}

std::string Autoindex::AutoindexStream(const std::string &DirectoryName) {

	std::vector<std::string> names = createListing(DirectoryName);

	if (_errCode != CODE_200)
		return "";

	std::string html = createHTML(DirectoryName, names);

	if (_errCode != CODE_200)
		return "";

	return html;
}
