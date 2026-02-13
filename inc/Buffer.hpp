#pragma once

#include "CompileTimeConstants.hpp"
#include <string>
#include <sys/types.h>

/// \class Buffer
/// \ brief This is a simple character buffer that can be filled from
/// and emptied into file descriptors.
///
/// Maybe some of the function names are not fully intuitive.
/// Make sure to read the comments.
///
/// You can make a string from a buffers content by calling
/// std::string s(buf.begin(), buf.end());
class Buffer {
public:
  Buffer(void);
  Buffer(const Buffer &other);
  Buffer &operator=(const Buffer &other);
  ~Buffer(void);

  typedef size_t size_type;
  typedef char *iterator;
  typedef const char *const_iterator;

  char &operator[](size_type i);
  const char &operator[](size_type i) const;

  iterator begin(void);
  const_iterator begin(void) const;

  iterator end(void);
  const_iterator end(void) const;

  const static size_type size = BUFFER_SIZE;

  /// \brief get currently occupied storage in Buffer
  size_type getOccupied(void) const;

  /// \brief gets currently meaningfully occupied storage in Buffer
  size_type getUsed(void) const;

  /// \brief gets currently free storage in Buffer
  size_type getFree(void) const;

  /// \brief get storage that is blocked by garbage
  size_type getBlocked(void) const;

  /// \brief reads up to Bytes bytes from Fd and fills them into the Buffer
  ///
  /// This is non blocking.
  ///
  /// on error throws exception, otherwise returns amount of bytes filled
  ///
  /// \return amounts of bytes filled into buffer
  size_type fill(const int Fd, const size_t Bytes);

  /// \brief reads Bytes bytes from Buffer and sends them to Fd
  ///
  /// This is non blocking.
  /// When SIGPIPE would be sent instead an exception is thrown.
  ///
  /// on error throws exception, otherwise returns amount of bytes emptied into
  /// Fd
  ///
  /// \return amounts of bytes emptied into Fd
  size_type empty(const int Fd, const size_t Bytes);

  /// \brief moves meaningful data to the front of the buffer.
  void format(void);

  /// \brief formats buffer if it is deemed wise for performance gains
  ///
  /// So far this is an extremely simple algorithm
  void optimize(const size_t Bytes);

  /// \brief resets the buffer
  void reset(void);

  /// \deletes the first Bytes meaningful bytes from the buffer
  void deleteFront(const size_t Bytes);

private:
  char _buffer[BUFFER_SIZE];
  size_type _start;
  size_type _end;
};
