/// \file CompileTimeConstants.hpp
///
/// \brief Contains Values that should be fixed at compile time.
///
/// These are not just defaults which could be overwritten by a parsed config file.
/// Instead these are values that could have program wide repurcussions
/// but should not be changeable at runtime.
#pragma once

/// \def BUFFER_SIZE
/// \brief defines the size for class Buffer objects
#ifndef
#define BUFFER_SIZE 1024
#endif
