
#pragma once

#include <string>

std::string getCurrentTimeString();

std::string getTimeStringFromTimespec(const struct timespec Time, const char *fmt = "%a, %d %b %Y %H:%M:%S %Z");
