#pragma once

#define DEBUG_STREAM std::cout
#define DEBUG_COLOR ""
#define DEBUG_LABEL "[DEBUG]  "

#define INFO_STREAM std::cout
#define INFO_COLOR ""
#define INFO_LABEL "[INFO]   "

#define WARNING_STREAM std::cerr
#define WARNING_COLOR "\x1B[33m"
#define WARNING_LABEL "[WARNING]"

#define ERROR_STREAM std::cerr
#define ERROR_COLOR "\x1B[31m"
#define ERROR_LABEL "[ERROR]  "
