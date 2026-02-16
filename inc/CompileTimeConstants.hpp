/// \file CompileTimeConstants.hpp
///
/// \brief Contains Values that should be fixed at compile time.
///
/// These are not just defaults which could be overwritten by a parsed config
/// file. Instead these are values that could have program wide repurcussions
/// but should not be changeable at runtime.
#pragma once

/// \def BUFFER_SIZE
/// \brief defines the size for class Buffer objects
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/// \def BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_1
/// \brief used by Buffer::optimize()
///
/// If want to know what it does exactly, read code and comments of
/// Buffer::optimize()
///
/// Generally speaking:
/// The smaller it is, the more frequently the buffer will get formatted
///
/// Should never be below 1
#ifndef BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_1
#define BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_1 8
#endif // BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_1

/// \def BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2
/// \brief used by Buffer::optimize()
///
/// If want to know what it does exactly, read code and comments of
/// Buffer::optimize()
///
/// Generally speaking:
/// The bigger it is, the more frequently the buffer will get formatted
///
/// Should never be below 1
#ifndef BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2
#define BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2 2
#endif // BUFFER_OPTIMIZE_THRESHOLD_DIVISOR_2
