#include "Time.hpp"
#include <ctime>
#include <string>

#define DATE_LENGTH 80

std::string getCurrentTimeString()
{
    time_t rawtime;
    time(&rawtime);

    const struct tm* timeinfo = gmtime(&rawtime);

    char buffer[DATE_LENGTH];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeinfo);

    return (std::string)buffer;
}

std::string getTimeStringFromTimespec(const struct timespec Time, const char *fmt){
	char buffer[DATE_LENGTH];
	strftime(buffer, sizeof(buffer), fmt, gmtime(&Time.tv_sec));
	return (std::string)buffer;
}
