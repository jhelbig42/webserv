#include "Autoindex.hpp"
#include "Logging.hpp"
#include "StatusCodes.hpp"
#include <errno.h>
#include <iostream>
#include <sstream>

Autoindex::Autoindex()
	:  _errCode(CODE_200){
}

Autoindex::~Autoindex(){
	_entries.clear();
}

Autoindex::Autoindex(const Autoindex &Other)
	:_entries(Other._entries), _errCode(Other._errCode){
}

Autoindex &Autoindex::operator=(const Autoindex &Other){
	_entries = Other._entries;
	_errCode = Other._errCode;
	return *this;
}

std::string Autoindex::AutoindexStream(std::string DirectoryName){
	DIR *dirp = opendir(DirectoryName.c_str());
	if (dirp == NULL){
		_errCode = CODE_500; // internal server error
		return "";
	}
	// fill vector with entries
	errno = 0;
	dirent *entry = readdir(dirp);

	while (entry){
		logging::log(logging::Debug, "directory entry found");
		if (entry->d_name[0] != '.')
			_entries.push_back(entry);
		entry = readdir(dirp);
	}
	// if end of dirstream is reached entry == NULL, but errno remains unchanged
	if (errno){
		_errCode = CODE_500;
		return "";
	}
	// put entries into stringstream
	std::stringstream sAutoindex;
	sAutoindex << "listing of the files" << std::endl;
	for (std::vector<dirent *>::iterator it = _entries.begin(); it != _entries.end(); ++it){
		sAutoindex << "file name: " << (*it)->d_name 
			<< " size in Bytes: " << (*it)->d_reclen << std::endl;
	}
	sAutoindex << "end of test" << std::endl;
	closedir(dirp);
	return sAutoindex.str();
}
