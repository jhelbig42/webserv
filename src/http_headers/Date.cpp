#include "Date.hpp"
#include <ctime>
#include <string>

#define DATE_LENGTH 80

std::string getDate()
{
    time_t rawtime;
    time(&rawtime);

    const struct tm* timeinfo = gmtime(&rawtime);

    char buffer[DATE_LENGTH];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeinfo);

    return std::string(buffer);
}
