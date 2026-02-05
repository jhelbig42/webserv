#pragma once

#define DEBUG_STREAM std::cout
#define DEBUG_COLOR ""
#define DEBUG_LABEL "[DEBUG]  "

#define LOG_STREAM std::cout
#define LOG_COLOR ""
#define LOG_LABEL "[LOG]    "

#define WARNING_STREAM std::cerr
#define WARNING_COLOR "\x1B[33m"
#define WARNING_LABEL "[WARNING]"

#define ERROR_STREAM std::cerr
#define ERROR_COLOR "\x1B[31m"
#define ERROR_LABEL "[ERROR]  "
