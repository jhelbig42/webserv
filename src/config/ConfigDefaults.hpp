/// \file Defaults.hpp
///
/// \brief defines default values for several configuration options
///
/// should be included by Config.cpp
#pragma once

/// \def DEFAULT_LOG_LEVEL
///
/// \brief sets the default log level
///
/// should be one of the following:
/// logging::DEBUG, logging::INFO, logging::WARNING, logging::ERROR
#ifndef LOG_LEVEL
#define LOG_LEVEL logging::Info
#endif // DEFAULT_LOG_LEVEL

#ifndef LOG_COLORED
#define LOG_COLORED true
#endif // LOG_USE_COLOR
