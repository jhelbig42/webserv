#include "Date.hpp"

std::string getDate()
{
    time_t rawtime;
    time(&rawtime);

    struct tm* timeinfo = localtime(&rawtime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeinfo);

    return std::string(buffer);
}
