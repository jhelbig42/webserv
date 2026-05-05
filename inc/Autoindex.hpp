#pragma once

#include <dirent.h>
#include <string>
#include <vector>

/*
Autoindex shall give back an html file or stringstream that contains an html file
thi file shall contain the index generated for this specific directory
on error NULL shall be returned and a specific errorcode be set (likely 500)
*/
class Autoindex{

private:
	std::vector<dirent *> _entries; //dynamic sized array of dirent structs as read from readdir()
	int _errCode;

public:
	Autoindex();
	Autoindex (const Autoindex &Other);
	Autoindex &operator=(const Autoindex &Other);
	~Autoindex();

	std::string AutoindexStream(std::string DirectoryName);
	

};
