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
/// log::DEBUG, log::INFO, log::WARNING, log::ERROR
#ifndef DEFAULT_LOG_LEVEL
#define DEFAULT_LOG_LEVEL log::Info
#endif //DEFAULT_LOG_LEVEL
